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
#include "core/data/DataSet.h"
#include "gui/MainWin.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/SideBar.h"

#include <QSignalBlocker>

DataList DataComboBox::_data_sets;
QVector<DataComboBox*> DataComboBox::_all_combos;

DataComboBox::DataComboBox(QWidget* parent) : QComboBox(parent), _current_index(0)
{
    _all_combos.push_back(this);
    connect(
        this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &DataComboBox::onDataChanged);
}

DataComboBox::~DataComboBox()
{
    _all_combos.erase(std::find(_all_combos.begin(), _all_combos.end(), this));
}

void DataComboBox::addDataSet(const ohkl::sptrDataSet& data)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(data->name()));
    if (!_data_sets.empty()) // Init buffer for the first data set
        data->initBuffer(true);
    _data_sets.push_back(data);
    refresh();
}

void DataComboBox::addDataSets(const DataList& data_list)
{
    QSignalBlocker blocker(this);
    for (auto data : data_list)
        addDataSet(data);
    if (gSession->currentProject()->hasDataSet())
        if (!gSession->currentProject()->currentData())
            gSession->currentProject()->setCurrentData(currentData());
}

//! Clear all elements
void DataComboBox::clearAll()
{
    QSignalBlocker blocker(this);
    _current_text = currentText();
    _current_index = currentIndex();
    clear();
    _data_sets.clear();
}

//! Return a pointer to the current unit cell
ohkl::sptrDataSet DataComboBox::currentData()
{
    if (count() != _data_sets.size())
        throw std::runtime_error("DataComboBox needs refreshing");
    _current_index = currentIndex();
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

void DataComboBox::syncAll()
{
    for (auto* combo : _all_combos) {
        QSignalBlocker blocker(combo);
        combo->setCurrentIndex(_current_index);
    }
}

void DataComboBox::onDataChanged(int index)
{
    if (!gSession->currentProject()->hasDataSet() || index == _current_index)
        return;

    auto old_data = _data_sets.at(_current_index);
    auto new_data = _data_sets.at(index);
    old_data->clearBuffer();
    new_data->initBuffer(true);
    _current_index = index;
    syncAll();
    gSession->currentProject()->setCurrentData(currentData());
    gSession->onDataChanged();
}
