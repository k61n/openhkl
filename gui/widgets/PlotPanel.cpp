//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/PlotPanel.cpp
//! @brief     Implements class PlotPanel
//!
//! @homepage  https://openhkl.org
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
#include <gsl/gsl_histogram.h>

PlotPanel::PlotPanel() : _yLog(false)
{
    _anchor = new QHBoxLayout(this);
    _plot = new SXPlot(this);
    _anchor->addWidget(_plot);
    _centralWidget = this;
    connect(_plot, &SXPlot::signalXRange, this, &PlotPanel::signalXRangeChanged);
    connect(_plot, &SXPlot::signalYRange, this, &PlotPanel::signalYRangeChanged);
}

void PlotPanel::plotData(
    QVector<double>& x, QVector<double>& y, QVector<double>& e, QString xtitle, QString ytitle,
    int xmin, int xmax, int ymin, int ymax)
{
    if (_plot->getType().compare("simple") != 0) {
        _anchor->removeWidget(_plot);
        delete _plot;
        _plot = PlotFactory::instance().create("simple", _centralWidget);
        _plot->setObjectName("1D plotter");
        _plot->setFocusPolicy(Qt::StrongFocus);
        // Thanks to the complicated lifetime of the _plot member, we need to
        // ensure the connection is made every time we instantiate it.
        connect(_plot, &SXPlot::signalXRange, this, &PlotPanel::signalXRangeChanged);
        connect(_plot, &SXPlot::signalYRange, this, &PlotPanel::signalYRangeChanged);
        _anchor->addWidget(_plot);
    }
    _plot->clearPlottables();
    _plot->addGraph();

    QPen pen(QColor(0, 0, 0));
    pen.setWidth(1);
    _plot->graph(0)->setPen(pen);

    _plot->graph(0)->setLineStyle(QCPGraph::LineStyle::lsLine);
    _plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    _plot->graph(0)->setData(x, y);
    _plot->addErrorBars(_plot->graph(0), e);

    if ((xmin != -1 && xmax != -1))
        _plot->xAxis->setRange(xmin, xmax);

    if ((ymin != -1 && ymax != -1))
        _plot->yAxis->setRange(ymin, ymax);

    if ((xmin == -1 && xmax == -1) && (ymin == -1 && ymax == -1))
        _plot->rescaleAxes();

    if (!xtitle.isEmpty())
        _plot->xAxis->setLabel(xtitle);

    if (!ytitle.isEmpty())
        _plot->yAxis->setLabel(ytitle);

    _plot->replot();
}

void PlotPanel::plotData(
    gsl_histogram* histogram, QString xtitle, QString ytitle, int xmin, int xmax, int ymin,
    int ymax)
{
    if (!histogram)
        return;

    QVector<double> x;
    QVector<double> y;
    QVector<double> e;

    x.resize(histogram->n);
    y.resize(histogram->n);

    memcpy(x.data(), histogram->range, histogram->n * sizeof(double));
    memcpy(y.data(), histogram->bin, histogram->n * sizeof(double));

    plotData(x, y, e, xtitle, ytitle, xmin, xmax, ymin, ymax);
}

void PlotPanel::plotHistogram(
    size_t nData, double* range, double* bin, QString xtitle, QString ytitle, int xmin, int xmax,
    int ymin, int ymax)
{
    // need to convert Data for QCustomplot
    QVector<double> frequency;
    QVector<double> counts;

    frequency.resize(nData);
    counts.resize(nData);

    memcpy(frequency.data(), range, nData * sizeof(double));
    memcpy(counts.data(), bin, nData * sizeof(double));

    QSharedPointer<QCPBarsDataContainer> data;

    _plot->clearPlottables();
    _plot->addGraph();
    QCPBars* bars = new QCPBars(_plot->xAxis, _plot->yAxis);
    bars->setName("Intensity Histogram");
    bars->setData(frequency, counts);
    _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    _plot->setFocusPolicy(Qt::StrongFocus);
    _anchor->addWidget(_plot);

    _plot->xAxis->setLabel(xtitle);
    _plot->yAxis->setLabel(ytitle);

    if ((xmin != -1 && xmax != -1))
        _plot->xAxis->setRange(xmin, xmax);

    if ((ymin != -1 && ymax != -1))
        _plot->yAxis->setRange(ymin, ymax);

    if ((xmin == -1 && xmax == -1) && (ymin == -1 && ymax == -1))
        _plot->rescaleAxes();

    _plot->replot();
}


void PlotPanel::setYLog(bool on)
{
    if (on == _yLog)
        return;
    _yLog = on;
    if (_yLog) {
        _plot->yAxis->grid()->setSubGridVisible(true);
        _plot->xAxis->grid()->setSubGridVisible(true);
        _plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        _plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
        QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
        _plot->yAxis->setTicker(logTicker);
        _plot->yAxis2->setTicker(logTicker);
        _plot->yAxis->setNumberFormat("eb");
        _plot->yAxis->setNumberPrecision(0);
    } else {
        _plot->yAxis->grid()->setSubGridVisible(true);
        _plot->xAxis->grid()->setSubGridVisible(true);
        _plot->yAxis->setScaleType(QCPAxis::stLinear);
        _plot->yAxis2->setScaleType(QCPAxis::stLinear);
        QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
        _plot->yAxis->setTicker(ticker);
        _plot->yAxis2->setTicker(ticker);
        _plot->yAxis->setNumberFormat("eb");
        _plot->yAxis->setNumberPrecision(0);
    }
}

void PlotPanel::updatePlot(PlottableItem* item)
{
    if (!item)
        return;

    if (!item->isPlottable(_plot)) {
        _anchor->removeWidget(_plot);
        delete _plot;
        _plot = PlotFactory::instance().create(item->getPlotType(), _centralWidget);
        // Thanks to the complicated lifetime of the _plot member, we need to
        // ensure the connection is made every time we instantiate it.
        connect(_plot, &SXPlot::signalXRange, this, &PlotPanel::signalXRangeChanged);
        connect(_plot, &SXPlot::signalYRange, this, &PlotPanel::signalYRangeChanged);
        _plot->setObjectName("1D plotter");
        _plot->setFocusPolicy(Qt::StrongFocus);
        _anchor->addWidget(_plot);
    }

    item->plot(_plot);
    // update();
}

void PlotPanel::exportPlot()
{
    // _plot->exportToAscii();
}
