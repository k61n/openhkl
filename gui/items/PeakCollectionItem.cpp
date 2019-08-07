//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/items/PeakCollectionItem.cpp
//! @brief     The visual item of the peak with a pointer to the peak object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/items/PeakCollectionItem.h"

PeakCollectionItem::PeakCollectionItem(nsx::PeakCollection* peak_collection)
    :QStandardItem()
{
    _peak_collection = peak_collection;

    std::vector<nsx::Peak3D*>* peak_list = _peak_collection->getPeakList();
    for (nsx::Peak3D* peak : *peak_list)
    {
        QStandardItem* item = new PeakItem(peak); 
        appendRow(item);
    }

}

int PeakCollectionItem::childCount() const
{
    return rowCount();
}

int PeakCollectionItem::columnCount() const
{
    return 11;
}

QVariant PeakCollectionItem::data(
    const QModelIndex &index, 
    int role) const
{
    int row = index.row();
    return _peak_items[row].data(index, role);

}

