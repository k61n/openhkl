//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogCalibrateDistance.cpp
//! @brief     Implements class DialogCalibrateDistance
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "apps/dialogs/DialogCalibrateDistance.h"
#include "ui_DialogCalibrateDistance.h"

DialogCalibrateDistance::DialogCalibrateDistance(QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogCalibrateDistance)
{
    ui->setupUi(this);
}

DialogCalibrateDistance::~DialogCalibrateDistance()
{
    delete ui;
}
