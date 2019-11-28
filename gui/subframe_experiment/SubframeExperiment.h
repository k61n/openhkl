//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeExperiment.h
//! @brief     Defines class SubframeExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_EXPERIMENT_SUBFRAME_EXPERIMENT_H
#define GUI_SUBFRAME_EXPERIMENT_SUBFRAME_EXPERIMENT_H

#include "gui/subframe_experiment/ImagePanel.h"
#include "gui/subframe_experiment/LoggerPanel.h"
#include "gui/subframe_experiment/PlotPanel.h"
#include "gui/subframe_experiment/PropertyPanel.h"

#include <QWidget>

class SubframeExperiment : public QWidget {

public:

    SubframeExperiment();

    ImagePanel* getImage() {return _image;};
    LoggerPanel* getLogger() {return _logger;};
    PlotPanel* getPlot() {return _plot;};
    PropertyPanel* getProperty() {return _properties;};

private:

    ImagePanel* _image;
    LoggerPanel* _logger;
    PlotPanel* _plot;
    PropertyPanel* _properties;
};

#endif // GUI_SUBFRAME_EXPERIMENT_SUBFRAME_EXPERIMENT_H
