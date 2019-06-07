//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subframe_experiments.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H
#define NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H

#include <QCR/widgets/views.h>

//! Part of the main window that controls the data and correction files.
class SubframeExperiments : public QcrDockWidget {
public:
    SubframeExperiments();
    void addExperiment(const QString&, const QString&);
    void removeExperiment();
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H
