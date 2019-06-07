//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/PeakListItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QIcon>

#include "PeakListItem.h"
#include "PeakListPropertyWidget.h"

PeakListItem::PeakListItem(const nsx::PeakList& peaks) : InspectableTreeItem(), _peaks(peaks)
{
    setText("Peaks");

    setEditable(true);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);

    setCheckable(true);
}

QWidget* PeakListItem::inspectItem()
{
    return new PeakListPropertyWidget(this);
}

void PeakListItem::removePeak(nsx::sptrPeak3D peak)
{
    auto it = std::find(_peaks.begin(), _peaks.end(), peak);
    if (it != _peaks.end()) {
        _peaks.erase(it);
    }
}

nsx::PeakList& PeakListItem::peaks()
{
    return _peaks;
}
