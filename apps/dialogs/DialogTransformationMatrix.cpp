//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogTransformationMatrix.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/crystal/UnitCell.h"

#include "apps/dialogs/DialogTransformationMatrix.h"
#include "ui_DialogTransformationMatrix.h"

DialogTransformationMatrix::DialogTransformationMatrix(nsx::sptrUnitCell unitCell, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogTransformationMatrix), _unitCell(unitCell)
{
    ui->setupUi(this);
}

DialogTransformationMatrix::~DialogTransformationMatrix()
{
    delete ui;
}

void DialogTransformationMatrix::accept()
{
    Eigen::Matrix3d P;
    P << ui->P00->value(), ui->P01->value(), ui->P02->value(), ui->P10->value(), ui->P11->value(),
        ui->P12->value(), ui->P20->value(), ui->P21->value(), ui->P22->value();

    _unitCell->transform(P);
}
