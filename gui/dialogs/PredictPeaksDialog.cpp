//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/PredictPeaksDialog.cpp
//! @brief     Implements class PredictPeaksDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/PredictPeaksDialog.h"

#include "core/experiment/DataSet.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/models/Session.h"
#include "gui/models/SessionExperiment.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

PredictPeaksDialog::PredictPeaksDialog()
    : QDialog{}
{
    if (gSession->selectedExperimentNum() < 0) {
        qWarning() << "Call of predict peaks without selected experiment";
        return;
    }
    if (!gSession->selectedExperiment()->getLibrary()) {
        qWarning() << "Cannot predict peaks without a shape library";
        return;
    }

    layout();
}

void PredictPeaksDialog::layout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* upperlayout = new QHBoxLayout;
    QFormLayout* leftlayout = new QFormLayout;

    unitCells = new QcrComboBox("adhoc_unitCellPredDialog", new QcrCell<int>(0), []() {
        return gSession->selectedExperiment()->getUnitCellNames();
    });
    dmin = new QcrDoubleSpinBox("adhoc_dminPredPeaks", new QcrCell<double>(1.5), 5, 2);
    dmax = new QcrDoubleSpinBox("adhoc_dmaxPredPeaks", new QcrCell<double>(50.0), 5, 2);
    minNeighbors = new QcrSpinBox("adhoc_minNeighborPredPeaks", new QcrCell<int>(20), 5);
    radius = new QcrDoubleSpinBox("adhoc_radiusPredPeaks", new QcrCell<double>(400.0), 5, 2);
    nFrames = new QcrDoubleSpinBox("adhoc_nFramesPredPeaks", new QcrCell<double>(100.0), 5, 2);
    interpolation = new QcrComboBox("adhoc_interpolationPredPeaks", new QcrCell<int>(0),
    {"No interpolation", "Inverse distance", "Intensity"});
    leftlayout->addRow("unit cell", unitCells);
    leftlayout->addRow("d min (angstrom)", dmin);
    leftlayout->addRow("d max (angstrom)", dmax);
    leftlayout->addRow("minimum neighbors", minNeighbors);
    leftlayout->addRow("search radius", radius);
    leftlayout->addRow("interpolation", interpolation);
    upperlayout->addLayout(leftlayout);
    preview = new PeaksTableView;
    model = new PeaksTableModel(
                "adhoc_PredictedPeaksModel", gSession->selectedExperiment()->experiment());
    preview->setModel(model);
    upperlayout->addWidget(preview);
    layout->addLayout(upperlayout);
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|
                                                     QDialogButtonBox::Apply, Qt::Horizontal);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::clicked, [=](QAbstractButton* button) {
        QDialogButtonBox::StandardButton role = buttons->standardButton(button);
        switch (role) {
        case QDialogButtonBox::StandardButton::Apply: {
            predict();
            break;
        }
        case QDialogButtonBox::StandardButton::Ok: {
            accept();
            break;
        }
        default: {
            reject();
            break;
        }
        }
    });
}

void PredictPeaksDialog::accept()
{
    if (!peaks.empty()) {
        std::unique_ptr<ListNameDialog> dlg(new ListNameDialog(peaks));

        if (!dlg->exec()) {
            QDialog::accept();
            return;
        }

        Peaks* pred = new Peaks(peaks, dlg->listName(), listtype::PREDICTED, "-");
        const QString& selName = gSession->selectedExperiment()->selectedListName();
        pred->file_ = QString::fromStdString(
                    gSession->selectedExperiment()->allData().at(0)->filename());

        gSession->selectedExperiment()->addPeaks(pred, selName);
    }

    QDialog::accept();
}

void PredictPeaksDialog::predict()
{
    qDebug() << "Starting peak prediction...";

    QList<nsx::sptrDataSet> data = gSession->selectedExperiment()->allData();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    nsx::sptrShapeLibrary lib = gSession->selectedExperiment()->getLibrary();
    nsx::sptrUnitCell cell = gSession->selectedExperiment()->getUnitCell(unitCells->currentIndex());
    double mind = dmin->value();
    double maxd = dmax->value();
    double searchradius = radius->value();
    double frames = nFrames->value();
    int neighbors = minNeighbors->value();
    int interpol = interpolation->currentIndex();

    nsx::PeakInterpolation peak_interpolation = static_cast<nsx::PeakInterpolation>(interpol);
    nsx::PeakList predictedPeaks;

    int current_numor = 0;
    for (nsx::sptrDataSet d : data) {
        qDebug() << "Predicting peaks for numor " << ++current_numor << " of " << data.size();

        auto&& predicted = nsx::predictPeaks(
            *lib, d, cell, mind, maxd, searchradius, frames, neighbors, peak_interpolation);

        for (nsx::sptrPeak3D peak : predicted)
            predictedPeaks.push_back(peak);

        qDebug() << "Added " << predicted.size() << " predicted peaks.";
    }

    peaks = predictedPeaks;
    qDebug() << "Completed  peak prediction. Added " << predictedPeaks.size() << " peaks";
    model->setPeaks(peaks);
}
