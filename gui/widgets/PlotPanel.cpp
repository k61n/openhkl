//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/PlotPanel.cpp
//! @brief     Implements class PlotPanel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/PlotPanel.h"

#include "gui/MainWin.h"
#include "gui/actions/Actions.h"
#include "gui/graphics/PlotFactory.h"
#include "gui/graphics/SXPlot.h"
#include "gui/graphics_items/PlottableItem.h"
#include "qcustomplot.h"

PlotPanel::PlotPanel() : _yLog(false)
{
    anchor = new QHBoxLayout(this);
    plot = new SXPlot(this);
    anchor->addWidget(plot);
    centralWidget = this;
}

void PlotPanel::plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e, QString xtitle, QString ytitle, int xmin, int xmax, int ymin, int ymax)
{
    if (plot->getType().compare("simple") != 0) {
        anchor->removeWidget(plot);
        delete plot;
        plot = PlotFactory::instance().create("simple", centralWidget);
        plot->setObjectName("1D plotter");
        plot->setFocusPolicy(Qt::StrongFocus);
        anchor->addWidget(plot);
    }
    plot->clearPlottables();
    plot->addGraph();

    QPen pen(QColor(0,0,0));
    pen.setWidth(1);
    plot->graph(0)->setPen(pen);

    plot->graph(0)->setLineStyle(QCPGraph::LineStyle::lsLine);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    plot->graph(0)->setData(x, y);
    plot->addErrorBars(plot->graph(0), e);

    if ((xmin != -1 && xmax != -1))
        plot->xAxis->setRange(xmin, xmax);

    if ((ymin != -1 && ymax != -1))
        plot->yAxis->setRange(ymin, ymax);

    if ((xmin == -1 && xmax == -1) && (ymin == -1 && ymax == -1))
        plot->rescaleAxes();

    if (!xtitle.isEmpty())
        plot->xAxis->setLabel(xtitle);

    if (!ytitle.isEmpty())
        plot->yAxis->setLabel(ytitle);

    plot->replot();
}

void PlotPanel::plotHistogram(size_t nData, double* range, double* bin, QString xtitle, QString ytitle, int xmin, int xmax, int ymin, int ymax)
{
    // need to convert Data for QCustomplot
    QVector<double> frequency;
    QVector<double> counts;

    frequency.resize(nData);
    counts.resize(nData);

    memcpy(frequency.data(), range, nData * sizeof(double));
    memcpy(counts.data(), bin, nData * sizeof(double));

    QSharedPointer<QCPBarsDataContainer> data;

    plot->clearPlottables();
    plot->addGraph();
    QCPBars *bars = new QCPBars(plot->xAxis, plot->yAxis);
    bars->setName("Intensity Histogram");
    bars->setData(frequency, counts);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plot->setFocusPolicy(Qt::StrongFocus);
    anchor->addWidget(plot);

    plot->xAxis->setLabel(xtitle);
    plot->yAxis->setLabel(ytitle);

    if ((xmin != -1 && xmax != -1))
        plot->xAxis->setRange(xmin, xmax);

    if ((ymin != -1 && ymax != -1))
        plot->yAxis->setRange(ymin, ymax);

    if ((xmin == -1 && xmax == -1) && (ymin == -1 && ymax == -1))
        plot->rescaleAxes();

    plot->replot();
}


void PlotPanel::setYLog(bool on)
{
    if (on == _yLog) return;
    _yLog = on;
    if (_yLog){
        plot->yAxis->grid()->setSubGridVisible(true);
        plot->xAxis->grid()->setSubGridVisible(true);
        plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
        QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
        plot->yAxis->setTicker(logTicker);
        plot->yAxis2->setTicker(logTicker);
        plot->yAxis->setNumberFormat("eb");
        plot->yAxis->setNumberPrecision(0);
    }
    else {
        plot->yAxis->grid()->setSubGridVisible(true);
        plot->xAxis->grid()->setSubGridVisible(true);
        plot->yAxis->setScaleType(QCPAxis::stLinear);
        plot->yAxis2->setScaleType(QCPAxis::stLinear);
        QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
        plot->yAxis->setTicker(ticker);
        plot->yAxis2->setTicker(ticker);
        plot->yAxis->setNumberFormat("eb");
        plot->yAxis->setNumberPrecision(0);
    }
}

void PlotPanel::updatePlot(PlottableItem* item)
{
    if (!item)
        return;

    if (!item->isPlottable(plot)) {
        anchor->removeWidget(plot);
        delete plot;
        plot = PlotFactory::instance().create(item->getPlotType(), centralWidget);
        plot->setObjectName("1D plotter");
        plot->setFocusPolicy(Qt::StrongFocus);
        anchor->addWidget(plot);
    }

    item->plot(plot);
    // update();
}

void PlotPanel::exportPlot()
{
    // plot->exportToAscii();
}
