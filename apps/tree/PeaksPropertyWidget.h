//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/PeaksPropertyWidget.h
//! @brief     Defines class PeaksPropertyWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QStandardItem>
#include <QWidget>

#include "apps/models/CollectedPeaksModel.h"

#include "apps/views/PeakTableView.h"

namespace Ui {

class PeaksPropertyWidget;
}

class PeaksItem;

class PeaksPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit PeaksPropertyWidget(PeaksItem* caller, QWidget* parent = 0);

    ~PeaksPropertyWidget();

    CollectedPeaksModel* model();

private:
    PeaksItem* _peaks_item;

    Ui::PeaksPropertyWidget* ui;

    CollectedPeaksModel* _peaks_model;
};
