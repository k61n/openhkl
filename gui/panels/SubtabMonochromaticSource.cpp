//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubtabMonochromaticSource.cpp
//! @brief     Implements class SubtabMonochromatic
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/SubtabMonochromaticSource.h"
#include "gui/properties/SourceProperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabMonochromatic

SubtabMonochromatic::SubtabMonochromatic() : QcrWidget {"monochromatic source"}
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget((property = new SourceProperty));
    setLayout(layout);
}
