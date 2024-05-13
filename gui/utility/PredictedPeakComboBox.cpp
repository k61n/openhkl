//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/PredictedPeakComboBox.h
//! @brief     Defines class PredictedPeakComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/PredictedPeakComboBox.h"

#include "core/shape/PeakCollection.h"

#include <QSignalBlocker>

PeakList PredictedPeakComboBox::_predicted_peaks;

PredictedPeakComboBox::PredictedPeakComboBox(QWidget* parent) : PeakComboBox(parent)
{
    _list_pointer = &_predicted_peaks;
}

void PredictedPeakComboBox::addPeakCollection(ohkl::PeakCollection* peaks)
{
    if (peaks->type() != ohkl::PeakCollectionType::PREDICTED)
        return;
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(peaks->name()));
    _predicted_peaks.push_back(peaks);
    refresh();
}

void PredictedPeakComboBox::addPeakCollections(const PeakList& peaks)
{
    for (auto collection : peaks)
        addPeakCollection(collection);
}
