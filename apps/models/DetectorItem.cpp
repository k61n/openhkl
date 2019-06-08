//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/DetectorItem.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QIcon>

#include "apps/models/DetectorItem.h"
#include "apps/tree/DetectorPropertyWidget.h"
#include "apps/frames/FrameDetectorGlobalOffsets.h"

DetectorItem::DetectorItem() : InspectableTreeItem()
{
    setText("Detector");

    QIcon icon(":/resources/detectorIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(false);
}

QWidget* DetectorItem::inspectItem()
{
    return new DetectorPropertyWidget(this);
}

void DetectorItem::openDetectorGlobalOffsetsFrame()
{
    FrameDetectorGlobalOffsets* frame = FrameDetectorGlobalOffsets::create(experimentItem());

    frame->show();

    frame->raise();
}
