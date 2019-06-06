
#include "gui/graphics/peakplot.h"

NSXPlot* PeakPlot::create(QWidget* parent)
{
    return new PeakPlot(parent);
}

PeakPlot::PeakPlot(QWidget* parent)
    : NSXPlot(parent)
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
    graph(0)->setErrorType(QCPGraph::etBoth);
    graph(0)->setLineStyle(QCPGraph::lsLine);
    graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));

    xAxis->setLabel("Frame (a.u.)");
    yAxis->setLabel("Intensity (counts)");

    //
    setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    QCPPlotTitle* element = new QCPPlotTitle(this, "");
    element->setFont(QFont("Arial", 8, -1, true));
    plotLayout()->addElement(0, 0, element);
}

std::string PeakPlot::getType() const
{
    return "peak";
}

