//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/EmptyPropertyWidget.h
//! @brief     Defines class EmptyPropertyWidget
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

class EmptyPropertyWidget;
}

class EmptyPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit EmptyPropertyWidget(QWidget* parent = 0);
    ~EmptyPropertyWidget();

private:
    Ui::EmptyPropertyWidget* ui;
};
