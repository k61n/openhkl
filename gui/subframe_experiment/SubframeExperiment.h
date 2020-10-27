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

#include <QWidget>

class ImagePanel;
class LoggerPanel;
class PlotPanel;
class PropertyPanel;

class SubframeExperiment : public QWidget {
 public:
    SubframeExperiment();

    ImagePanel* getImage() { return _image; };
    LoggerPanel* getLogger() { return _logger; };
    PlotPanel* getPlot() { return _plot; };
    PropertyPanel* getProperty() { return _properties; };

 private:
    ImagePanel* _image;
    LoggerPanel* _logger;
    PlotPanel* _plot;
    PropertyPanel* _properties;
};

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
