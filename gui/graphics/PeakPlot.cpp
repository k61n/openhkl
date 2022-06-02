//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/PeakPlot.cpp
//! @brief     Implements class PeakPlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/PeakPlot.h"

SXPlot* PeakPlot::create(QWidget* parent)
{
    return new PeakPlot(parent);
}

PeakPlot::PeakPlot(QWidget* parent) : SXPlot(parent)
{
    plotLayout()->insertRow(0);
    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    xAxis->setTickLabelFont(font);
    yAxis->setTickLabelFont(font);

    addGraph();
    graph(0)->setPen(pen);
    graph(0)->setLineStyle(QCPGraph::lsLine);
    graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
    xErrorBars_ = new QCPErrorBars{graph(0)->keyAxis(), graph(0)->valueAxis()};
    yErrorBars_ = new QCPErrorBars{graph(0)->keyAxis(), graph(0)->valueAxis()};
    xErrorBars_->setDataPlottable(graph(0));
    yErrorBars_->setDataPlottable(graph(0));
    xErrorBars_->setPen(pen);
    yErrorBars_->setPen(pen);
    xErrorBars_->setErrorType(QCPErrorBars::ErrorType::etKeyError);
    yErrorBars_->setErrorType(QCPErrorBars::ErrorType::etValueError);

    xAxis->setLabel("Frame (a.u.)");
    yAxis->setLabel("Intensity (counts)");

    //
    setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    QCPTextElement* element = new QCPTextElement(this, "");
    element->setFont(QFont("Arial", 8, -1, true));
    plotLayout()->addElement(0, 0, element);
}

std::string PeakPlot::getType() const
{
    return "peak";
}
