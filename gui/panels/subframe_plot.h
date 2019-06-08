//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_plot.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_SUBFRAME_PLOT_H
#define NSXGUI_GUI_PANELS_SUBFRAME_PLOT_H

#include <QCR/widgets/views.h>

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframePlot : public QcrDockWidget {
public:
    SubframePlot();
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_PLOT_H
