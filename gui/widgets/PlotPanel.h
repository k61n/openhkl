//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#ifndef NSX_GUI_SUBFRAME_EXPERIMENT_PLOTPANEL_H
#define NSX_GUI_SUBFRAME_EXPERIMENT_PLOTPANEL_H

#include <QHBoxLayout>
#include <QWidget>

class PlottableItem;
class SXPlot;

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

#endif // NSX_GUI_SUBFRAME_EXPERIMENT_PLOTPANEL_H
