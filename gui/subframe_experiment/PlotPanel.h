//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframePlot.h
//! @brief     Defines class SubframePlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_EXPERIMENT_PLOT_PANEL_H
#define GUI_SUBFRAME_EXPERIMENT_PLOT_PANEL_H

#include "gui/graphics_items/PlottableItem.h"
#include <QCR/widgets/views.h>
#include <QHBoxLayout>

//! Part of the main window that contains the plot
class SubframePlot : public QcrWidget {
 public:
    SubframePlot();
    void plotData(QVector<double>&, QVector<double>&, QVector<double>&);
    void updatePlot(PlottableItem* item);
    void exportPlot();

 private:
    SXPlot* plot;
    QHBoxLayout* anchor;
    QcrWidget* centralWidget;
};

#endif // GUI_SUBFRAME_EXPERIMENT_PLOT_PANEL_H
