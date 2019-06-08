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

#include "core/instrument/Diffractometer.h"
#include "core/instrument/Experiment.h"
#include "core/instrument/Source.h"

#include "apps/models/SourceItem.h"
#include "apps/tree/SourcePropertyWidget.h"

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
