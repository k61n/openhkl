//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/PeakListPropertyWidget.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <memory>

#include <QStandardItem>
#include <QWidget>

#include "CollectedPeaksModel.h"
#include "PeakTableView.h"

namespace Ui {
class PeakListPropertyWidget;
}

class PeakListItem;

class PeakListPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit PeakListPropertyWidget(PeakListItem* caller, QWidget* parent = 0);

    ~PeakListPropertyWidget();

    CollectedPeaksModel* model();

private:
    PeakListItem* _caller;

    Ui::PeakListPropertyWidget* ui;

    CollectedPeaksModel* _peaks_model;
};
