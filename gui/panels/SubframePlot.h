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

#ifndef GUI_PANELS_SUBFRAMEPLOT_H
#define GUI_PANELS_SUBFRAMEPLOT_H

#include "gui/graphics/PlottableItem.h"
#include <QCR/widgets/views.h>
#include <QHBoxLayout>

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframePlot : public QcrDockWidget {
 public:
    SubframePlot();
    void plotData(QVector<double>&, QVector<double>&, QVector<double>&);
    void updatePlot(PlottableItem* item);
    void exportPlot();

 private:
    NSXPlot* plot;
    QHBoxLayout* anchor;
    QcrWidget* centralWidget;
};

#endif // GUI_PANELS_SUBFRAMEPLOT_H
