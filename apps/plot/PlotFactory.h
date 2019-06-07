//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/plot/PlotFactory.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <string>

#include <core/Factory.h>
#include <core/Singleton.h>

#include "SXPlot.h"

class QWidget;

class PlotFactory : public nsx::Factory<SXPlot, std::string, QWidget*>,
                    public nsx::Singleton<PlotFactory, nsx::Constructor, nsx::Destructor> {

private:
    friend class nsx::Constructor<PlotFactory>;
    friend class nsx::Destructor<PlotFactory>;
    PlotFactory();
    ~PlotFactory();
};
