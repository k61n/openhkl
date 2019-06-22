//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/plot/SXPlot.h
//! @brief     Defines class SXPlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "QCustomPlot.h"

class PlottableGraphicsItem;

class SXPlot : public QCustomPlot {
    Q_OBJECT
 public:
    static SXPlot* create(QWidget* parent);

    explicit SXPlot(QWidget* parent = 0);
    void keyPressEvent(QKeyEvent*);
    virtual ~SXPlot();

    // Getters and setters

    virtual std::string getType() const;

 public slots:
    //
    void mousePress();
    //
    void mouseWheel();
    // Update the plot
    virtual void update(PlottableGraphicsItem* item);
    void titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title);
    // Double click on the legend
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    // Context menu
    void setmenuRequested(QPoint);
    // Export the graphs into column ASCII format
    void exportToAscii();

 private:
    void copyViewToClipboard();
};
