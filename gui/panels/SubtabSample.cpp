//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubtabSample.cpp
//! @brief     Implements class SubtabSample
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/SubtabSample.h"
#include "gui/properties/SampleShapeProperties.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabSample

SubtabSample::SubtabSample() : QcrWidget {"sample"}
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget((property = new SampleShapeProperties));
    setLayout(layout);
}
