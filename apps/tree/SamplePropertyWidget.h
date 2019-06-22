//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/SamplePropertyWidget.h
//! @brief     Defines class SamplePropertyWidget
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

class SamplePropertyWidget;
}

class SampleItem;

class SamplePropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit SamplePropertyWidget(SampleItem* caller, QWidget* parent = 0);

    ~SamplePropertyWidget();

private:
    Ui::SamplePropertyWidget* _ui;

    SampleItem* _sampleItem;
};
