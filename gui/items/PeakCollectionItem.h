//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/items/PeakCollectionItem.h
//! @brief     Defines classes PeaksTableModel and PeakTableView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_ITEMS_PEAKCOLLECTIONITEM_H
#define OHKL_GUI_ITEMS_PEAKCOLLECTIONITEM_H

#include "gui/graphics_items/PeakItemGraphic.h"
#include "gui/items/PeakItem.h"

#include <QVariant>
#include <memory>
#include <string>

namespace ohkl {
class PeakCollection;
}

class PeakCollectionItem {
 public:
    PeakCollectionItem();
    PeakCollectionItem(const ohkl::PeakCollection* peak_collection);
    ~PeakCollectionItem() = default;

 public:
    //! Set the peak collection
    void setPeakCollection(const ohkl::PeakCollection* peak_collection);
    //! Retrieve the name of the collection if present
    std::string name() const;
    //! Retrieve the number of children of this item
    int childCount() const;
    //! Retrieve the number of columns of this item
    int columnCount() const;
    //! Retrieve the data of column and row
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    //! Get the peak collection pointer
    const ohkl::PeakCollection* peakCollection() const { return _peak_collection; };
    //! Get the peak items pointer
    std::vector<PeakItem*> peakItems() const;
    //! Returns the peak item at a given row
    PeakItem* peakItemAt(int row) const;
    //! The sorthing of the peaks
    void sort(int column, Qt::SortOrder order);
    //! Return the row according to the vis. peak item pointer
    int returnRowOfVisualItem(PeakItemGraphic* peak_graphic) const;
    //! Set the visual to filter mode
    void setFilterMode();
    //! Return number of peaks caught by filter
    int numberCaughtByFilter() const;
    //! Return number of peaks rejected by filter
    int numberRejectedByFilter() const;
    //! Return number of peaks
    int numberOfPeaks() const;
    //! Remove all data
    void reset();

 private:
    const ohkl::PeakCollection* _peak_collection;
    std::vector<std::unique_ptr<PeakItem>> _peak_items;
    PeakDisplayModes _mode;
};

#endif // OHKL_GUI_ITEMS_PEAKCOLLECTIONITEM_H
