//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/DataComboBox.h
//! @brief     Defines class DataComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/DataComboBox.h"

#include <QSignalBlocker>
#include <qcombobox.h>
#include <qobject.h>

DataList DataComboBox::_data_sets;
QVector<DataComboBox*> DataComboBox::_all_combos;

DataComboBox::DataComboBox(QWidget* parent) : QComboBox(parent)
{
    _all_combos.push_back(this);
    connect(
        this, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &DataComboBox::onDataChanged);
}

void DataComboBox::addDataSet(const ohkl::sptrDataSet& data)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(data->name()));
    _data_sets.push_back(data);
    if (_data_sets.size() == 1) // Init buffer for the first data set
        data->initBuffer(true);
    refresh();
}

void DataComboBox::addDataSets(const DataList& data_list)
{
    for (auto data : data_list)
        addDataSet(data);
}

//! Clear all elements
void DataComboBox::clearAll()
{
    QSignalBlocker blocker(this);
    _current_text = currentText();
    clear();
    _data_sets.clear();
}

//! Return a pointer to the current unit cell
ohkl::sptrDataSet DataComboBox::currentData() const
{
    if (count() != _data_sets.size())
        throw std::runtime_error("DataComboBox needs refreshing");
    return _data_sets.at(currentIndex());
}

void DataComboBox::refresh()
{
    QSignalBlocker blocker(this);
    _current_text = currentText();
    clear();
    for (ohkl::sptrDataSet& data : _data_sets)
        addItem(QString::fromStdString(data->name()));
    setCurrentText(_current_text);
}

void DataComboBox::refreshAll()
{
    for (auto* combo : _all_combos)
        combo->refresh();
}

void DataComboBox::onDataChanged(int index)
{
    if (index != _current_index) {
        auto old_data = _data_sets.at(_current_index);
        auto new_data = _data_sets.at(index);
        old_data->clearBuffer();
        new_data->initBuffer(true);
        _current_index = index;
    }
}
