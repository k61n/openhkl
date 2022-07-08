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

PlotPanel::PlotPanel()
{
    anchor = new QHBoxLayout(this);
    plot = new SXPlot(this);
    anchor->addWidget(plot);
    centralWidget = this;
}

void PlotPanel::plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e)
{
    if (plot->getType().compare("simple") != 0) {
        anchor->removeWidget(plot);
        delete plot;
        plot = PlotFactory::instance().create("simple", centralWidget);
        plot->setObjectName("1D plotter");
        plot->setFocusPolicy(Qt::StrongFocus);
        anchor->addWidget(plot);
    }
    plot->graph(0)->setData(x, y);
    plot->addErrorBars(plot->graph(0), e);
    plot->rescaleAxes();
    plot->replot();
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
