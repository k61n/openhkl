#include "Plot/PeakPlot.h"

SXPlot* PeakPlot::create(QWidget *parent)
{
    return new PeakPlot(parent);
}

PeakPlot::PeakPlot(QWidget *parent) : SXPlot(parent)
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

    addGraph();
    graph(1)->setPen(QPen(QColor("magenta")));
    graph(1)->setBrush(QBrush(QColor(255,0,255,60)));

    addGraph();
    graph(2)->setPen(QPen(QColor("blue")));
    graph(2)->setBrush(QBrush(QColor(0,100,255,40)));

    xAxis->setLabel("Frame (a.u.)");
    yAxis->setLabel("Intensity (counts)");

    // Setup legends
    legend->setVisible(true);
    legend->setFont(QFont("Helvetica",9));
    graph(0)->setName("Total");
    graph(1)->setName("Peak");
    graph(2)->setName("Bkg.");

    //
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    QCPPlotTitle* element=new QCPPlotTitle(this, "");
    element->setFont(QFont("Arial",8,-1,true));
    plotLayout()->addElement(0, 0, element);
}

std::string PeakPlot::getType() const
{
    return "peak";
}
