//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/PlotFactory.h
//! @brief     Defines class PlotFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_PLOTFACTORY_H
#define GUI_GRAPHICS_PLOTFACTORY_H

#include "base/utils/Factory.h"
#include "base/utils/Singleton.h"
#include "gui/graphics/SXPlot.h"

class PlotFactory : public nsx::Factory<SXPlot, std::string, QWidget*>,
                    public nsx::Singleton<PlotFactory, nsx::Constructor, nsx::Destructor> {
 private:
    PlotFactory();
    ~PlotFactory();
    friend class nsx::Constructor<PlotFactory>;
    friend class nsx::Destructor<PlotFactory>;
};

#endif // GUI_GRAPHICS_PLOTFACTORY_H
