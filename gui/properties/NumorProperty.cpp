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

#include "gui/properties/NumorProperty.h"

#include "core/experiment/DataSet.h"
#include "core/experiment/DataTypes.h"
#include "core/raw/IDataReader.h"
#include "gui/models/Session.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>

NumorProperty::NumorProperty() : QcrWidget {"numorProperty"}
{
    QFormLayout* formLayout = new QFormLayout(this);
    table = new QTableWidget(this);

    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    numor = new QcrComboBox("adhoc_numors", new QcrCell<int>(0), []() {
        if (gSession->selectedExperimentNum() < 0)
            return QStringList {""};
        return gSession->selectedExperiment()->getDataNames();
    });
    numor->setHook([](int i) { gSession->selectedExperiment()->selectData(i); });
    formLayout->addRow("Data:", numor);
    formLayout->addRow(table);

    setRemake([this]() { onRemake(); });
    remake();
}

void NumorProperty::onRemake()
{
    clear();

    if (gSession->selectedExperimentNum() >= 0) {
        SessionExperiment* exp = gSession->selectedExperiment();
        nsx::sptrDataSet data = exp->getData();

        if (data) {
            const nsx::MetaData& metadata = data->reader()->metadata();
            const nsx::MetaDataMap& map = metadata.map();

            table->setColumnCount(2);
            table->setRowCount(map.size());

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
                table->setItem(numberLines, 0, col0);
                table->setItem(numberLines++, 1, col1);
            }
            table->horizontalHeader()->setStretchLastSection(true);
        }
    }
}

void NumorProperty::clear()
{
    table->removeColumn(1);
    table->removeColumn(0);
}
