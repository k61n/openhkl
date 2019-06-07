//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SampleItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QIcon>
#include <algorithm>

#include "DialogIsotopesDatabase.h"
#include "FrameSampleGlobalOffsets.h"
#include "SampleItem.h"
#include "SamplePropertyWidget.h"
#include "SampleShapeItem.h"

SampleItem::SampleItem() : InspectableTreeItem()
{
    setText("Sample");

    QIcon icon(":/resources/gonioIcon.png");

    setIcon(icon);

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(false);

    SampleShapeItem* shape = new SampleShapeItem();
    appendRow(shape);
}

QWidget* SampleItem::inspectItem()
{
    return new SamplePropertyWidget(this);
}

void SampleItem::openIsotopesDatabase()
{
    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogIsotopesDatabase> dialog_ptr(new DialogIsotopesDatabase());

    if (!dialog_ptr->exec()) {
        return;
    }
}

void SampleItem::openSampleGlobalOffsetsFrame()
{
    FrameSampleGlobalOffsets* frame = FrameSampleGlobalOffsets::create(experimentItem());

    frame->show();

    frame->raise();
}
