//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/PlotFactory.cpp
//! @brief     Implements class PlotFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/PlotFactory.h"

#include "gui/graphics/PeakPlot.h"
#include "gui/graphics/SimplePlot.h"

PlotFactory::PlotFactory()
{
    registerCallback("simple", &SimplePlot::create);
    registerCallback("peak", &PeakPlot::create);
}
