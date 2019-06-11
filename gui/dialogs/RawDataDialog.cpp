//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/RawDataDialog.cpp
//! @brief     Implements class RawDataDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/RawDataDialog.h"
#include "gui/MainWin.h"
#include <QFormLayout>

RawDataDialog::RawDataDialog()
    : QDialog(gGui)
{
    QFormLayout* layout = new QFormLayout(this);
    dataArrangement = new QcrComboBox("adhoc_dataarrangement",
                                      new QcrCell<int>(0),
                                      {"Column major", "Row major"});
    layout->addRow("Data arrangement", dataArrangement);
    dataFormat = new QcrComboBox("adhoc_dataformat",
                                 new QcrCell<int>(0),
                                 {"8 bit integer", "16 bit integer", "32 bit integer"});
    layout->addRow("Data format", dataFormat);
    swapEndianness = new QcrCheckBox("adhoc_swap", "Swap endianness", new QcrCell<bool>(true));
    layout->addRow(swapEndianness);
    chi = new QcrDoubleSpinBox("adhoc_rawChi", new QcrCell<double>(0.0), 5, 3);
    layout->addRow("delta chi", chi);
    omega = new QcrDoubleSpinBox("adhoc_rawOmega", new QcrCell<double>(0.0), 5, 3);
    layout->addRow("delta omega", omega);
    phi = new QcrDoubleSpinBox("adhoc_rawPhi", new QcrCell<double>(0.0), 5, 3);
    layout->addRow("delta phi", phi);
    wave = new QcrDoubleSpinBox("adhoc_rawWavelength", new QcrCell<double>(0.0), 5, 3);
    layout->addRow("wavelength", wave);
    buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                                   Qt::Horizontal, this);
    layout->addRow(buttons);

    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    show();
}

bool RawDataDialog::rowMajor()
{
    QString selection = dataArrangement->currentText();
    if (selection == "Row major")
        return true;
    if (selection == "Column major")
        return false;
    gLogger->log("[ERROR] RawDataDialog: unrecognized data arrangement!");
    return false;
}

int RawDataDialog::bpp()
{
    switch (dataFormat->currentIndex()) {
    case 0: //8 bit
        return 1;
    case 1: //16 bit
        return 2;
    case 2: //32 bit
        return 4;
    default:
        gLogger->log("[ERROR] RawDataDialog: unrecognized data format!");
        return -1;
    }
}
