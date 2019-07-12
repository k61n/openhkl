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
    show();
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
    minisigma = new QcrDoubleSpinBox("adhoc_minisigmaPredPeaks", new QcrCell<double>(0.0), 5, 2);
    radius = new QcrDoubleSpinBox("adhoc_radiusPredPeaks", new QcrCell<double>(400.0), 5, 2);
    nFrames = new QcrDoubleSpinBox("adhoc_nFramesPredPeaks", new QcrCell<double>(100.0), 5, 2);
    interpolation = new QcrComboBox("adhoc_interpolationPredPeaks", new QcrCell<int>(0), {""});
    leftlayout->addRow("unit cell", unitCells);
    leftlayout->addRow("d min (angstrom)", dmin);
    leftlayout->addRow("d max (angstrom)", dmax);
    leftlayout->addRow("minimum neighbors", minNeighbors);
    leftlayout->addRow("minimum I/Sigma", minisigma);
    leftlayout->addRow("search radius", radius);
    leftlayout->addRow("interpolation", interpolation);
    upperlayout->addLayout(leftlayout);
    preview = new PeaksTableView;
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
        case QDialogButtonBox::StandardButton::Cancel: {
            reject();
            break;
        }
        }
    });
}

void PredictPeaksDialog::accept()
{
    if (peaks.empty()) {
        QDialog::accept();
        return;
    }

}

void PredictPeaksDialog::predict()
{

}
