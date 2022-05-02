//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/PeakComboBox.h
//! @brief     Defines class PeakComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/PeakComboBox.h"

#include "core/shape/PeakCollection.h"
#include "gui/utility/LinkedComboBox.h"

#include <QSignalBlocker>

PeakList PeakComboBox::_peak_collections;

PeakComboBox::PeakComboBox(QWidget* parent) : QComboBox(parent)
{
}

void PeakComboBox::addPeakCollection(nsx::PeakCollection* peaks)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(peaks->name()));
    _peak_collections.push_back(peaks);
    refresh();
}

void PeakComboBox::addPeakCollections(const PeakList& peaks)
{
    for (auto collection : peaks)
        addPeakCollection(collection);
}

//! Clear all elements
void PeakComboBox::clearAll()
{
    QSignalBlocker blocker(this);
    clear();
    _peak_collections.clear();
}

//! Return a pointer to the current unit cell
nsx::PeakCollection* PeakComboBox::currentPeakCollection() const
{
    if (count() != _peak_collections.size())
        throw std::runtime_error("PeakComboBox needs refreshing");
    return _peak_collections.at(currentIndex());
}

void PeakComboBox::refresh()
{
    QSignalBlocker blocker(this);
    clear();
    for (nsx::PeakCollection* collection : _peak_collections)
        addItem(QString::fromStdString(collection->name()));
}
