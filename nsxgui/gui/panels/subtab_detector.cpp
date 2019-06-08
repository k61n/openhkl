//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subtab_detector.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/subtab_detector.h"
#include "nsxgui/gui/properties/detectorproperty.h"
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
