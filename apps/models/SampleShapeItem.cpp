//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SampleShapeItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "SampleShapeItem.h"
#include "SampleShapePropertyWidget.h"

SampleShapeItem::SampleShapeItem() : InspectableTreeItem()
{
    QIcon icon(":/resources/sampleIcon.png");
    setIcon(icon);

    setText("Shape");

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(false);
}

QWidget* SampleShapeItem::inspectItem()
{
    return new SampleShapePropertyWidget(this);
}
