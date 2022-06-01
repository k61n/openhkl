//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/FoundPeakComboBox.h
//! @brief     Defines class FoundPeakComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/FoundPeakComboBox.h"

#include "core/shape/PeakCollection.h"
#include "gui/utility/PeakComboBox.h"

#include <QSignalBlocker>

PeakList FoundPeakComboBox::_found_peaks;

FoundPeakComboBox::FoundPeakComboBox(QWidget* parent) : PeakComboBox(parent)
{
    _list_pointer = &_found_peaks;
}

void FoundPeakComboBox::addPeakCollection(nsx::PeakCollection* peaks)
{
    if (peaks->type() != nsx::PeakCollectionType::FOUND)
        return;
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(peaks->name()));
    _found_peaks.push_back(peaks);
    refresh();
}

void FoundPeakComboBox::addPeakCollections(const PeakList& peaks)
{
    for (auto collection : peaks)
        addPeakCollection(collection);
}
