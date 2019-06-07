
#include "nsxgui/gui/properties/numorproperty.h"
#include "nsxgui/gui/models/session.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <build/core/include/core/DataSet.h>
#include <build/core/include/core/DataTypes.h>
#include <build/core/include/core/IDataReader.h>

NumorProperty::NumorProperty() : QcrWidget {"numorProperty"}
{
    QGridLayout* gridLayout = new QGridLayout(this);
    table = new QTableWidget(this);
    QHBoxLayout* box = new QHBoxLayout();

    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QLabel* dlabel = new QLabel("Data:", this);
    box->addWidget(dlabel);

    label = new QLabel(this);
    box->addWidget(label);

    gridLayout->addLayout(box, 0, 0, 1, 1);
    gridLayout->addWidget(table, 1, 0, 1, 1);

    setRemake([this]() { onRemake(); });
    remake();
}

void NumorProperty::onRemake()
{
    clear();

    if (gSession->selectedExperimentNum() >= 0) {
        ExperimentModel* exp = gSession->selectedExperiment();
        auto data = exp->data()->selectedData();

        if (data) {
            label->setText(QString::fromStdString(data->filename()));

            const auto& metadata = data->reader()->metadata();
            const auto& map = metadata.map();

            table->setColumnCount(2);
            table->setRowCount(map.size());

            int numberLines = 0;
            for (auto element : map) // Only int, double and string metadata are displayed.
            {
                QTableWidgetItem* col0 = new QTableWidgetItem();
                QTableWidgetItem* col1 = new QTableWidgetItem();
                col0->setData(Qt::EditRole, QString(element.first));

                if (element.second.is<int>()) {
                    col1->setData(Qt::EditRole, element.second.as<int>());
                } else if (element.second.is<double>()) {
                    col1->setData(Qt::EditRole, element.second.as<double>());
                } else if (element.second.is<std::string>()) {
                    col1->setData(
                        Qt::EditRole,
                        QString(QString::fromStdString(element.second.as<std::string>())));
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
    label->setText("");
}
