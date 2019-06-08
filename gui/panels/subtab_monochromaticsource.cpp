//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subtab_monochromaticsource.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/subtab_monochromaticsource.h"
#include "gui/properties/sourceproperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabMonochromatic

SubtabMonochromatic::SubtabMonochromatic() : QcrWidget {"monochromatic source"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new SourceProperty));
    setLayout(layout);
}
