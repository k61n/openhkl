//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/NumorPropertyWidget.h
//! @brief     Defines class NumorPropertyWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QWidget>

namespace Ui {

class NumorPropertyWidget;
}

class NumorItem;

class NumorPropertyWidget : public QWidget {
    Q_OBJECT

 public:
    explicit NumorPropertyWidget(NumorItem* caller, QWidget* parent = 0);
    ~NumorPropertyWidget();

 private:
    Ui::NumorPropertyWidget* ui;
    NumorItem* _numorItem;
};
