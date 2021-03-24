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
#include <QFormLayout>

RawDataDialog::RawDataDialog()
{
    QFormLayout* layout = new QFormLayout(this);

    dataArrangement = new QComboBox();
    dataFormat = new QComboBox();
    swapEndianness = new QCheckBox("Swap endian");
    chi = new QDoubleSpinBox();
    omega = new QDoubleSpinBox();
    phi = new QDoubleSpinBox();
    wave = new QDoubleSpinBox();
    buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    dataArrangement->addItems(QStringList{"Column major", "Row major"});
    dataFormat->addItems(QStringList{"8 bit integer", "16 bit integer", "32 bit integer"});
    chi->setDecimals(3);
    phi->setDecimals(3);
    omega->setDecimals(3);
    wave->setDecimals(3);

    // default to Row major/16 bit
    dataArrangement->setCurrentIndex(1);
    dataFormat->setCurrentIndex(1);
    swapEndianness->setCheckState(Qt::Checked);

    layout->addRow("Data arrangement", dataArrangement);
    layout->addRow("Data format", dataFormat);
    layout->addRow(swapEndianness);
    layout->addRow("delta chi", chi);
    layout->addRow("delta omega", omega);
    layout->addRow("delta phi", phi);
    layout->addRow("wavelength", wave);
    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &RawDataDialog::accept);

    connect(buttons, &QDialogButtonBox::rejected, this, &RawDataDialog::reject);
}

bool RawDataDialog::rowMajor()
{
    QString selection = dataArrangement->currentText();
    if (selection == "Row major")
        return true;
    if (selection == "Column major")
        return false;
    return false;
}

int RawDataDialog::bpp()
{
    switch (dataFormat->currentIndex()) {
        case 0: // 8 bit
            return 1;
        case 1: // 16 bit
            return 2;
        case 2: // 32 bit
            return 4;
        default: return -1;
    }
}
