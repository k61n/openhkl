//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/EmptyPropertyWidget.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "ui_EmptyPropertyWidget.h"

#include "apps/tree/EmptyPropertyWidget.h"

EmptyPropertyWidget::EmptyPropertyWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::EmptyPropertyWidget)
{
    ui->setupUi(this);
}

EmptyPropertyWidget::~EmptyPropertyWidget()
{
    delete ui;
}
