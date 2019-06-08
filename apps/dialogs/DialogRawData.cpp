//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogRawData.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/logger/Logger.h"

#include "apps/dialogs/DialogRawData.h"
#include "ui_DialogRawData.h"

DialogRawData::DialogRawData(QWidget* parent) : QDialog(parent), ui(new Ui::DialogRawData)
{
    ui->setupUi(this);
}

DialogRawData::~DialogRawData()
{
    delete ui;
}

double DialogRawData::wavelength()
{
    return ui->wavelength->value();
}

double DialogRawData::deltaChi()
{
    return ui->deltaChi->value();
}

double DialogRawData::deltaOmega()
{
    return ui->deltaOmega->value();
}

double DialogRawData::deltaPhi()
{
    return ui->deltaPhi->value();
}

bool DialogRawData::swapEndian()
{
    return ui->swapEndian->isChecked();
}

bool DialogRawData::rowMajor()
{
    QString selection = ui->dataArrangement->currentText();

    if (selection == "Row major")
        return true;
    else if (selection == "Column major")
        return false;
    else {
        nsx::error() << "unrecognized data arrangement!!";
        return false;
    }
}

int DialogRawData::bpp()
{
    switch (ui->dataFormat->currentIndex()) {
    case 0:
        // 8-bit
        return 1;
    case 1:
        // 16-bit
        return 2;
    case 2:
        // 32-bit
        return 4;
    default: nsx::error() << "unrecognized data format!!"; return -1;
    }
}

void DialogRawData::setWavelength(double new_wavelength)
{
    ui->wavelength->setValue(new_wavelength);
}
