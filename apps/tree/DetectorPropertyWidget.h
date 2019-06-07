//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/DetectorPropertyWidget.h
//! @brief     Defines ###CLASSES###
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
class DetectorPropertyWidget;
}

class DetectorItem;

class DetectorPropertyWidget : public QWidget {

    Q_OBJECT

public:
    explicit DetectorPropertyWidget(DetectorItem* caller, QWidget* parent = nullptr);

    ~DetectorPropertyWidget();

private:
    void onSampleToDectorDistanceChanged(double distance);

private:
    Ui::DetectorPropertyWidget* _ui;

    DetectorItem* _detectorItem;
};
