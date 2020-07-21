//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/PlotPanel.h
//! @brief     Defines class PlotPanel
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
#include <QHBoxLayout>
#include <QWidget>

//! Part of the main window that contains the plot
class PlotPanel : public QWidget {
 public:
    PlotPanel();
    void plotData(QVector<double>&, QVector<double>&, QVector<double>&);
    void updatePlot(PlottableItem* item);
    void exportPlot();

 private:
    SXPlot* plot;
    QHBoxLayout* anchor;
    QWidget* centralWidget;
};

#endif // GUI_SUBFRAME_EXPERIMENT_PLOT_PANEL_H
