//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subtab_sample.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/subtab_sample.h"
#include "gui/properties/sampleshapeproperties.h"
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
