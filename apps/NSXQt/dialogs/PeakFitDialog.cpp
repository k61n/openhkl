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

#include "dialogs/PeakFitDialog.h"
#include "ui_PeakFitDialog.h"
#include "models/SessionModel.h"

#include "IData.h"

using namespace SX::Crystal;

PeakFitDialog::PeakFitDialog(SessionModel* session, QWidget *parent) :
    QDialog(parent),
    _session(session),
    ui(new Ui::PeakFitDialog)
{
    ui->setupUi(this);
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
        ui->hSpinBox->setValue(0);
        ui->kSpinBox->setValue(0);
        ui->lSpinBox->setValue(0);
        _peakFit = nullptr;
        return false;
    }

    _peakFit = std::unique_ptr<PeakFit>(new PeakFit(_peak));

    return true;
}

void PeakFitDialog::updatePlots()
{
    // not implemented yet
}

void PeakFitDialog::on_hSpinBox_valueChanged(int arg1)
{
    if ( changePeak() )
        updatePlots();
}

void PeakFitDialog::on_kSpinBox_valueChanged(int arg1)
{
    if ( changePeak() )
        updatePlots();
}

void PeakFitDialog::on_lSpinBox_valueChanged(int arg1)
{
    if ( changePeak() )
        updatePlots();
}
