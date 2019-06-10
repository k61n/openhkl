//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      ###FILE###
//! @brief     Implements class SimplePlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/graphics/simpleplot.h"

NSXPlot* SimplePlot::create(QWidget* parent)
{
    return new SimplePlot(parent);
}

SimplePlot::SimplePlot(QWidget* parent) : NSXPlot(parent)
{
    plotLayout()->insertRow(0);
    addGraph();
    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    xAxis->setTickLabelFont(font);
    yAxis->setTickLabelFont(font);

    graph(0)->setPen(pen);
    graph(0)->setErrorType(QCPGraph::etValue);
    graph(0)->setLineStyle(QCPGraph::lsLine);
    graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));

    //    xAxis->setLabel("Frame (a.u.)");
    //    yAxis->setLabel("Intensity (counts)");

    //    // Setup legends
    //    legend->setVisible(true);
    //    legend->setFont(QFont("Helvetica",9));
    //    graph(0)->setName("Total");

    //
    setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    QCPPlotTitle* element = new QCPPlotTitle(this, "");
    element->setFont(QFont("Arial", 8, -1, true));
    plotLayout()->addElement(0, 0, element);
}

std::string SimplePlot::getType() const
{
    return "simple";
}
