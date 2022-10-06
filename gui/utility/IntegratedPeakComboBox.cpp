//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/IntegratedPeakComboBox.h
//! @brief     Defines class IntegratedPeakComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/IntegratedPeakComboBox.h"

#include "core/shape/PeakCollection.h"
#include "gui/utility/PeakComboBox.h"

#include <QSignalBlocker>

PeakList IntegratedPeakComboBox::_integrated_peaks;

IntegratedPeakComboBox::IntegratedPeakComboBox(QWidget* parent) : PeakComboBox(parent)
{
    _list_pointer = &_integrated_peaks;
}

void IntegratedPeakComboBox::addPeakCollection(ohkl::PeakCollection* peaks)
{
    if (!peaks->isIntegrated())
        return;
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(peaks->name()));
    _integrated_peaks.push_back(peaks);
    refresh();
}

void IntegratedPeakComboBox::addPeakCollections(const PeakList& peaks)
{
    if (_empty_first && !itemText(0).isEmpty())
        addItem(QString());
    for (auto collection : peaks)
        addPeakCollection(collection);
}
