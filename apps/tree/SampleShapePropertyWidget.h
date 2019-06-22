//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/SampleShapePropertyWidget.h
//! @brief     Defines class SampleShapePropertyWidget
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

class SampleShapePropertyWidget;
}

class SampleShapeItem;

class SampleShapePropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit SampleShapePropertyWidget(SampleShapeItem* caller, QWidget* parent = 0);
    ~SampleShapePropertyWidget();

private slots:
    void on_pushButton_LoadMovie_clicked();
    void setHullProperties();

private:
    SampleShapeItem* _caller;
    Ui::SampleShapePropertyWidget* ui;
};
