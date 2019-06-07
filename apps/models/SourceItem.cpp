//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SourceItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QIcon>

#include <core/Diffractometer.h>
#include <core/Experiment.h>
#include <core/Source.h>

#include "SourceItem.h"
#include "SourcePropertyWidget.h"

SourceItem::SourceItem(const char* name) : InspectableTreeItem()
{
    setText(name);

    QIcon icon(":/resources/sourceIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(false);
}

QWidget* SourceItem::inspectItem()
{
    return new SourcePropertyWidget(this);
}
