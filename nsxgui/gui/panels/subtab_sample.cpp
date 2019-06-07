//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subtab_sample.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/subtab_sample.h"
#include "nsxgui/gui/properties/sampleshapeproperties.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabSample

SubtabSample::SubtabSample() : QcrWidget {"sample"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new SampleShapeProperties));
    setLayout(layout);
}
