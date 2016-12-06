/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forshungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "dialogs/PeakFitDialog.h"
#include "ui_PeakFitDialog.h"
#include "models/SessionModel.h"
#include "ColorMap.h"
#include "IData.h"
#include "Minimizer.h"

using namespace SX::Crystal;
using namespace SX::Utils;

PeakFitDialog::PeakFitDialog(SessionModel* session, QWidget *parent) :
    QDialog(parent),
    _session(session),
    _peakImage(nullptr),
    _fitImage(nullptr),
    _differenceImage(nullptr),
    _chiSquaredImage(nullptr),
    ui(new Ui::PeakFitDialog)
{
    ui->setupUi(this);

    _peakScene = new QGraphicsScene(this);
    _fitScene = new QGraphicsScene(this);
    _differenceScene = new QGraphicsScene(this);
    _chiSquaredScene = new QGraphicsScene(this);

    ui->peakView->setScene(_peakScene);
    ui->fitView->setScene(_fitScene);
    ui->differenceView->setScene(_differenceScene);
    ui->chiSquaredView->setScene(_chiSquaredScene);
}

PeakFitDialog::~PeakFitDialog()
{
    delete ui;
}

bool PeakFitDialog::changePeak()
{
    int h, k, l;

    h = ui->hSpinBox->value();
    k = ui->kSpinBox->value();
    l = ui->lSpinBox->value();

    auto hkl = Eigen::Vector3i(h, k, l);

    bool peak_found = false;

    auto numors = _session->getSelectedNumors();

    for (int i = 0; i < numors.size() && !peak_found; ++i) {
        auto& peaks = numors[i]->getPeaks();

        for (auto peak: peaks) {
            Eigen::Vector3i peak_hkl = peak->getIntegerMillerIndices();

            if (hkl == peak_hkl) {
                _peak = peak;
                peak_found = true;
                break;
            }
        }
    }

    // could not find it in the list!
    if (!peak_found) {
        qCritical() << "Peak with specified HKL not found!";
        _peakFit = nullptr;
        _peak = nullptr;
        return false;
    }

    _peakFit = std::unique_ptr<PeakFit>(new PeakFit(_peak));
    _bestParams = _peakFit->defaultParams();

    Eigen::Vector3d lower = _peak->getBackground()->getLower();
    Eigen::Vector3d upper = _peak->getBackground()->getUpper();

    int min = std::max(0.0, std::ceil(lower(2)));
    int max = std::min(_peak->getData()->getNFrames()-1.0, std::floor(upper(2)));

    ui->frameScrollBar->setMinimum(min);
    ui->frameScrollBar->setMaximum(max);
    ui->frameScrollBar->setValue(std::round((min+max)/2.0));

    updatePlots();
    return true;
}

void PeakFitDialog::updatePlots()
{
    if (!_peak)
        return;

    int frame = ui->frameScrollBar->value();

    Eigen::ArrayXXd peakData = _peakFit->peakData(frame);
    Eigen::ArrayXXd predData = _peakFit->predict(_bestParams, frame);

    Eigen::ArrayXXd diffData = _peakFit->relDifference(frame);
    Eigen::ArrayXXd chi2Data = _peakFit->chi2(frame);

    const int max_intensity = std::round(_peakFit->maxIntensity());
    diffData *= max_intensity;

    QRect sceneRect(0, 0, peakData.cols()-1, peakData.rows()-1);
    _peakScene->setSceneRect(sceneRect);
    _fitScene->setSceneRect(sceneRect);
    _differenceScene->setSceneRect(sceneRect);
    _chiSquaredScene->setSceneRect(sceneRect);

    QImage peak = Mat2QImage(peakData, sceneRect, max_intensity);
    QImage pred = Mat2QImage(predData, sceneRect, max_intensity);
    QImage chi2 = Mat2QImage(chi2Data, sceneRect, max_intensity);
    QImage diff = Mat2QImage(diffData, sceneRect, max_intensity);

    if (!_peakImage) {
        _peakImage = _peakScene->addPixmap(QPixmap::fromImage(peak));
        _fitImage = _fitScene->addPixmap(QPixmap::fromImage(pred));
        _chiSquaredImage = _chiSquaredScene->addPixmap((QPixmap::fromImage(chi2)));
        _differenceImage = _differenceScene->addPixmap((QPixmap::fromImage(diff)));
    }
    else {
        _peakImage->setPixmap(QPixmap::fromImage(peak));
        _fitImage->setPixmap(QPixmap::fromImage(pred));
        _chiSquaredImage->setPixmap(QPixmap::fromImage(chi2));
        _differenceImage->setPixmap(QPixmap::fromImage(diff));
    }

    ui->peakView->fitInView(_peakScene->sceneRect(), Qt::KeepAspectRatio);
    ui->fitView->fitInView(_fitScene->sceneRect(), Qt::KeepAspectRatio);
    ui->chiSquaredView->fitInView(_chiSquaredScene->sceneRect(), Qt::KeepAspectRatio);
    ui->differenceView->fitInView(_differenceScene->sceneRect(), Qt::KeepAspectRatio);
}

void PeakFitDialog::on_hSpinBox_valueChanged(int arg1)
{
    changePeak();
}

void PeakFitDialog::on_kSpinBox_valueChanged(int arg1)
{
    changePeak();
}

void PeakFitDialog::on_lSpinBox_valueChanged(int arg1)
{
    changePeak();
}

void PeakFitDialog::on_frameScrollBar_valueChanged(int value)
{
    updatePlots();
}

void PeakFitDialog::on_runFitButton_clicked()
{
    Minimizer min;

    auto min_func = [&](const Eigen::VectorXd& par, Eigen::VectorXd& res) -> int
    {
        _peakFit->residuals(par, res);
        return 0;
    };



    Eigen::VectorXd params = _bestParams;

    min.initialize(_peakFit->numParams(), _peakFit->numValues());
    min.set_f(min_func);
    min.setParams(params);

    if (min.fit(100) ) {
        qDebug() << "Fit converged!";
    }
    else {
        qDebug() << "Fit did not converge! " << min.getStatusStr();
    }

    _bestParams = min.params();
    updatePlots();
}
