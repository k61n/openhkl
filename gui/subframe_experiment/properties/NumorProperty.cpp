//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/properties/NumorProperty.cpp
//! @brief     Implements class NumorProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/NumorProperty.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/raw/IDataReader.h"
#include "gui/MainWin.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>

NumorProperty::NumorProperty() : QWidget()
{
    setSizePolicies();

    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* top_layout = new QHBoxLayout;

    _numor_selector = new QComboBox(this);
    _add = new QPushButton(this);
    _remove = new QPushButton(this);
    _table = new QTableWidget(this);

    _add->setIcon(QIcon(":/images/Add_item.svg"));
    _remove->setIcon(QIcon(":/images/Delete_item.svg"));

    _table->horizontalHeader()->setVisible(false);
    _table->verticalHeader()->setVisible(false);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    _numor_selector->setSizePolicy(*_size_policy_widgets);

    top_layout->addWidget(_numor_selector);
    top_layout->addWidget(_add);
    top_layout->addWidget(_remove);

    layout->addLayout(top_layout);
    layout->addWidget(_table);

    connect(
        _numor_selector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &NumorProperty::onChanged);

    connect(_add, &QPushButton::clicked, this, &NumorProperty::addMenuRequested);
}

void NumorProperty::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}


void NumorProperty::addMenuRequested()
{
    QMenu* menu = new QMenu(_add);

    QAction* add_from_raw = menu->addAction("Add raw data...");
    QAction* add_from_HDF5 = menu->addAction("Add hdf5 or nexus data...");

    connect(add_from_raw, &QAction::triggered, []() { gSession->loadRawData(); });

    connect(add_from_HDF5, &QAction::triggered, []() { gSession->loadData(); });

    menu->popup(mapToGlobal(_add->geometry().bottomLeft()));
}

void NumorProperty::refreshInput()
{
    _numor_selector->blockSignals(true);
    _numor_selector->clear();
    _numor_selector->addItems(gSession->currentProject()->getDataNames());
    _numor_selector->blockSignals(false);

    if (!gSession->currentProject()->getDataNames().empty())
        onChanged();
}

void NumorProperty::onChanged()
{
    clear();

    if (gSession->currentProjectNum() >= 0) {
        Project* exp = gSession->currentProject();
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
                col0->setData(Qt::EditRole, QString(element.first.c_str()));

                if (std::holds_alternative<int>(element.second))
                    col1->setData(Qt::EditRole, std::get<int>(element.second));
                else if (std::holds_alternative<double>(element.second))
                    col1->setData(Qt::EditRole, std::get<double>(element.second));
                else if (std::holds_alternative<std::string>(element.second)) {
                    col1->setData(
                        Qt::EditRole,
                        QString::fromStdString(std::get<std::string>(element.second)));
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
