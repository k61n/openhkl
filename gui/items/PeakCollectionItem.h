//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/items/PeakCollectionItem.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_ITEMS_PEAKCOLLECTIONITEM_H
#define GUI_ITEMS_PEAKCOLLECTIONITEM_H

#include "core/peak/PeakCollection.h"
#include "gui/items/PeakItem.h"
#include "core/peak/Peak3D.h"
#include <QStandardItem>

class PeakCollectionItem : public QStandardItem {

public: 
    PeakCollectionItem(nsx::PeakCollection* peak_collection);
    ~PeakCollectionItem() = default;

public: 
    //! Retrieve the number of children of this item
    int childCount() const;
    //! Retrieve the number of columns of this item
    int columnCount() const;
    //! Retrieve the data of column and row
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //! Get the peak collection pointer
    nsx::PeakCollection* peakCollection() const {return _peak_collection;};
    //! Returns the peak item at a given row
    PeakItem* peakItemAt(int row) const;

private:
    nsx::PeakCollection* _peak_collection;
    std::vector<PeakItem*> _peak_items;

};

#endif // GUI_ITEMS_PEAKCOLLECTIONITEM_H