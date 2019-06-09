//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/WidgetRefinerFit.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <map>

#include <QWidget>

#include "core/experiment/DataTypes.h"
#include "core/refine/Refiner.h"

namespace Ui {
class WidgetRefinerFit;
}

class WidgetRefinerFit : public QWidget {

    Q_OBJECT

public:
    WidgetRefinerFit(const std::map<nsx::sptrDataSet, nsx::Refiner>& refiners);

    ~WidgetRefinerFit();

private slots:

    void slotSelectedDataChanged(int selected_data);

    void slotSelectedBatchChanged(int selected_batch);

    void slotSelectedFrameChanged(int selected_frame);

private:
    Ui::WidgetRefinerFit* _ui;

    std::map<nsx::sptrDataSet, nsx::Refiner> _refiners;
};
