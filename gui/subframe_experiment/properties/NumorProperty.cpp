//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/NumorProperty.cpp
//! @brief     Implements class NumorProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/NumorProperty.h"

#include "core/experiment/DataSet.h"
#include "core/experiment/DataTypes.h"
#include "core/raw/IDataReader.h"
#include "gui/models/Session.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>

NumorProperty::NumorProperty() : QWidget()
{
    QGridLayout* grid_layout = new QGridLayout(this);

    _table = new QTableWidget(this);

    _table->horizontalHeader()->setVisible(false);
    _table->verticalHeader()->setVisible(false);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    _numor_selector = new QComboBox();

    grid_layout->addWidget(_numor_selector, 0, 0, 1, 1);
    grid_layout->addWidget(_table, 1, 0, 1, 1);

    connect(
        _numor_selector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &NumorProperty::onChanged);
}

void NumorProperty::refreshInput()
{
    _numor_selector->blockSignals(true);
    _numor_selector->clear();
    _numor_selector->addItems(gSession->selectedExperiment()->getDataNames());
    _numor_selector->blockSignals(false);

    if (!gSession->selectedExperiment()->getDataNames().isEmpty())
        onChanged();
}

void NumorProperty::onChanged()
{
    clear();

    if (gSession->selectedExperimentNum() >= 0) {
        SessionExperiment* exp = gSession->selectedExperiment();
        nsx::sptrDataSet data = exp->getData(_numor_selector->currentIndex());

        if (data) {
            const nsx::MetaData& metadata = data->reader()->metadata();
            const nsx::MetaDataMap& map = metadata.map();

            _table->setColumnCount(2);
            _table->setRowCount(map.size());

            int numberLines = 0;
            for (auto element : map) // Only int, double and string metadata are displayed.
            {
                QTableWidgetItem* col0 = new QTableWidgetItem();
                QTableWidgetItem* col1 = new QTableWidgetItem();
                col0->setData(Qt::EditRole, QString(element.first));

                if (element.second.is<int>())
                    col1->setData(Qt::EditRole, element.second.as<int>());
                else if (element.second.is<double>())
                    col1->setData(Qt::EditRole, element.second.as<double>());
                else if (element.second.is<std::string>()) {
                    col1->setData(
                        Qt::EditRole, QString::fromStdString(element.second.as<std::string>()));
                } else {
                    delete col0;
                    delete col1;
                    continue;
                }
                _table->setItem(numberLines, 0, col0);
                _table->setItem(numberLines++, 1, col1);
            }
            _table->horizontalHeader()->setStretchLastSection(true);
        }
    }
}

void NumorProperty::clear()
{
    _table->removeColumn(1);
    _table->removeColumn(0);
}
