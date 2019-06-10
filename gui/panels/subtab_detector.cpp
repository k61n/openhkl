//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subtab_detector.cpp
//! @brief     Implements class SubtabDetector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/subtab_detector.h"
#include "gui/properties/detectorproperty.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class SubtabDetector

SubtabDetector::SubtabDetector() : QcrWidget {"detector"}
{
    auto* layout = new QHBoxLayout;
    layout->addWidget((property = new DetectorProperty));
    setLayout(layout);
}
