//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/plot/PlotFactory.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "PlotFactory.h"
#include "PeakPlot.h"
#include "SimplePlot.h"

PlotFactory::PlotFactory()
{
    registerCallback("simple", &SimplePlot::create);
    registerCallback("peak", &PeakPlot::create);
}

PlotFactory::~PlotFactory() {}
