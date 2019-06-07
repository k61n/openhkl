//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subtab_monochromaticsource.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/subtab_monochromaticsource.h"
#include "nsxgui/gui/properties/sourceproperty.h"
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
