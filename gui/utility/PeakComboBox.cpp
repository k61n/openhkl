//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/PeakComboBox.h
//! @brief     Defines class PeakComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/PeakComboBox.h"

#include "core/shape/PeakCollection.h"

#include <QSignalBlocker>

PeakList PeakComboBox::_peak_collections;
QVector<PeakComboBox*> PeakComboBox::_all_combos;

PeakComboBox::PeakComboBox(QWidget* parent) : QComboBox(parent)
{
    _list_pointer = &_peak_collections;
    _all_combos.push_back(this);
}

PeakComboBox::~PeakComboBox()
{
    _all_combos.erase(std::find(_all_combos.begin(), _all_combos.end(), this));
}

void PeakComboBox::addPeakCollection(ohkl::PeakCollection* peaks)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(peaks->name()));
    _list_pointer->push_back(peaks);
    refresh();
}

void PeakComboBox::addPeakCollections(const PeakList& peaks)
{
    if (_empty_first && !itemText(0).isEmpty())
        addItem(QString());
    for (auto collection : peaks)
        addPeakCollection(collection);
}

//! Clear all elements
void PeakComboBox::clearAll()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    _list_pointer->clear();
}

//! Return a pointer to the current unit cell
ohkl::PeakCollection* PeakComboBox::currentPeakCollection() const
{
    if (count() != _list_pointer->size()) {
        if (_empty_first) {
            if (currentIndex() == 0)
                return nullptr;
            return _list_pointer->at(currentIndex() - 1);
        }
        throw std::runtime_error("PeakComboBox needs refreshing");
    }
    return _list_pointer->at(currentIndex());
}

void PeakComboBox::refresh()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    if (_empty_first)
        addItem(QString());
    for (ohkl::PeakCollection* collection : *_list_pointer)
        addItem(QString::fromStdString(collection->name()));
    setCurrentText(_current);
}

void PeakComboBox::setEmptyFirst()
{
    _empty_first = true;
}

void PeakComboBox::refreshAll()
{
    for (auto* combo : _all_combos)
        combo->refresh();
}
