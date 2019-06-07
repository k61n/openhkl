//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_plot.cpp
//! @brief     Implements class SubframePlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/subframe_plot.h"
#include "gui/actions/triggers.h"
#include "gui/graphics/plotfactory.h"
#include "gui/mainwin.h"

//-------------------------------------------------------------------------------------------------
//! class SubframePlot

SubframePlot::SubframePlot() : QcrDockWidget {"Plotter"}
{
    centralWidget = new QcrWidget("adhoc_plotWidget");
    anchor = new QHBoxLayout(centralWidget);
    plot = new NSXPlot(this);
    anchor->addWidget(plot);
    setWidget(centralWidget);
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->triggers->viewPlotter,
        SLOT(setChecked(bool)));
}

void SubframePlot::plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e)
{
    if (plot->getType().compare("simple") != 0) {
        anchor->removeWidget(plot);
        delete plot;
        PlotFactory* factory = PlotFactory::Instance();
        plot = factory->create("simple", centralWidget);
        plot->setObjectName("1D plotter");
        plot->setFocusPolicy(Qt::StrongFocus);
        anchor->addWidget(plot);
    }
    plot->graph(0)->setDataValueError(x, y, e);
    plot->rescaleAxes();
    plot->replot();
}

void SubframePlot::updatePlot(PlottableItem* item)
{
    if (!item)
        return;

    if (!item->isPlottable(plot)) {
        anchor->removeWidget(plot);
        delete plot;
        PlotFactory* factory = PlotFactory::Instance();
        plot = factory->create(item->getPlotType(), centralWidget);
        plot->setObjectName("1D plotter");
        plot->setFocusPolicy(Qt::StrongFocus);
        anchor->addWidget(plot);
    }

    item->plot(plot);
    // update();
}

void SubframePlot::exportPlot()
{
    plot->exportToAscii();
}
