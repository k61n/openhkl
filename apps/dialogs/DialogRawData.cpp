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

 Forschungszentrum Juelich GmbH
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

#include <core/Logger.h>

#include "DialogRawData.h"
#include "ui_DialogRawData.h"

DialogRawData::DialogRawData(QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogRawData) {
    ui->setupUi(this);
}

DialogRawData::~DialogRawData() {
    delete ui;
}

double DialogRawData::wavelength() {
    return ui->wavelength->value();
}

double DialogRawData::deltaChi() {
    return ui->deltaChi->value();
}

double DialogRawData::deltaOmega() {
    return ui->deltaOmega->value();
}

double DialogRawData::deltaPhi() {
    return ui->deltaPhi->value();
}

bool DialogRawData::swapEndian() {
    return ui->swapEndian->isChecked();
}

bool DialogRawData::rowMajor() {
    QString selection = ui->dataArrangement->currentText();

    if ( selection == "Row major")
        return true;
    else if (selection == "Column major")
        return false;
    else {
        nsx::error() << "unrecognized data arrangement!!";
        return false;
    }
}

int DialogRawData::bpp() {
    switch(ui->dataFormat->currentIndex()) {
    case 0:
        // 8-bit
        return 1;
    case 1:
        // 16-bit
        return 2;
    case 2:
        // 32-bit
        return 4;
    default:
        nsx::error() << "unrecognized data format!!";
        return -1;
    }
}

void DialogRawData::setWavelength(double new_wavelength) {
    ui->wavelength->setValue(new_wavelength);
}

