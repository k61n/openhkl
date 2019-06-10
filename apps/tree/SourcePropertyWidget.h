//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/SourcePropertyWidget.h
//! @brief     Defines class SourcePropertyWidget
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
class SourcePropertyWidget;
}

class SourceItem;

class SourcePropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit SourcePropertyWidget(SourceItem* caller, QWidget* parent = 0);

    ~SourcePropertyWidget();

private:
    void onHeightChanged(double height);

    void onSelectedMonochromatorChanged(int index);

    void onWavelengthChanged(double wavelength);

    void onWidthChanged(double width);

private:
    Ui::SourcePropertyWidget* _ui;

    SourceItem* _caller;
};
