//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/DataComboBox.h
//! @brief     Defines class DataComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/DataComboBox.h"

#include <QSignalBlocker>
#include <qobject.h>

DataList DataComboBox::_data_sets;

DataComboBox::DataComboBox(QWidget* parent) : QComboBox(parent)
{
}

void DataComboBox::addDataSet(const nsx::sptrDataSet& data)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(data->name()));
    _data_sets.push_back(data);
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
    clear();
    _data_sets.clear();
}

//! Return a pointer to the current unit cell
nsx::sptrDataSet DataComboBox::currentData() const
{
    if (count() != _data_sets.size())
        throw std::runtime_error("DataComboBox needs refreshing");
    return _data_sets.at(currentIndex());
}

void DataComboBox::refresh()
{
    QSignalBlocker blocker(this);
    clear();
    for (nsx::sptrDataSet& data : _data_sets)
        addItem(QString::fromStdString(data->name()));
}
