//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/PlotFactory.h
//! @brief     Defines class PlotFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_PLOTFACTORY_H
#define OHKL_GUI_GRAPHICS_PLOTFACTORY_H

#include "base/utils/Factory.h"
#include "gui/graphics/SXPlot.h"

class SXPlot;

//! A creator for SXPlot%s
class PlotFactory : public ohkl::Factory<PlotFactory, SXPlot, std::string, QWidget*> {
 public:
    PlotFactory();
};

#endif // OHKL_GUI_GRAPHICS_PLOTFACTORY_H
