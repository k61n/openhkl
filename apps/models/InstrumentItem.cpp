//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/InstrumentItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QIcon>
#include <QString>

#include "core/instrument/Diffractometer.h"
#include "core/instrument/Source.h"

#include "apps/models/DetectorItem.h"
#include "apps/models/InstrumentItem.h"
#include "apps/models/SampleItem.h"
#include "apps/models/SourceItem.h"

InstrumentItem::InstrumentItem(const char* name, const char* source_name) : TreeItem()
{
    setText(name);

    QIcon icon(":/resources/instrumentIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);

    setCheckable(false);

    // Create a detector item and add it to the instrument item
    _detector = new DetectorItem();
    appendRow(_detector);

    // Create a sample item and add it to the instrument item
    _sample = new SampleItem();
    appendRow(_sample);

    // Create a source item and add it to the instrument leaf
    _source = new SourceItem(source_name);
    appendRow(_source);
}

DetectorItem* InstrumentItem::detectorItem()
{
    return _detector;
}

SampleItem* InstrumentItem::sampleItem()
{
    return _sample;
}

SourceItem* InstrumentItem::sourceItem()
{
    return _source;
}
