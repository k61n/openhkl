//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/SubframeExperiment.h
//! @brief     Defines class SubframeExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H

#include "core/data/DataTypes.h"

#include <QWidget>

class DetectorWidget;
class PlotPanel;
class PropertyPanel;

//! Frame containing information on all aspects of the experiment
class SubframeExperiment : public QWidget {
 public:
    SubframeExperiment();

    void dataChanged();
    DetectorWidget* detectorWidget();

    PlotPanel* getPlot() { return _plot; };
    PropertyPanel* getProperty() { return _properties; };

 private:
    std::vector<nsx::sptrDataSet> _data_list;
    DetectorWidget* _detector_widget;
    PlotPanel* _plot;
    PropertyPanel* _properties;
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
