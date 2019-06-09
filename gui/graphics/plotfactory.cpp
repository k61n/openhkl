//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      ###FILE###
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/plotfactory.h"
#include "gui/graphics/peakplot.h"
#include "gui/graphics/simpleplot.h"

PlotFactory::PlotFactory()
{
    registerCallback("simple", &SimplePlot::create);
    registerCallback("peak", &PeakPlot::create);
}

PlotFactory::~PlotFactory() {}
