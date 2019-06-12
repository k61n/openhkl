//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubtabDetector.cpp
//! @brief     Implements class SubtabDetector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/SubtabDetector.h"
#include "gui/properties/DetectorProperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabDetector

SubtabDetector::SubtabDetector() : QcrWidget {"detector"}
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget((property = new DetectorProperty));
    setLayout(layout);
}
