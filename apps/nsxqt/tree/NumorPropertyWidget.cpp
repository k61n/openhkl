#include <QWidget>

#include <nsxlib/DataSet.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/MetaData.h>

#include "MetaTypes.h"
#include "NumorItem.h"
#include "NumorPropertyWidget.h"

#include "ui_NumorPropertyWidget.h"

NumorPropertyWidget::NumorPropertyWidget(NumorItem* caller,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NumorPropertyWidget),
    _numorItem(caller)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto data = caller->data(Qt::UserRole).value<nsx::sptrDataSet>();
    if (!data) {
        return;
    }

    ui->label_Data->setText(QString::fromStdString(data->filename()));

    const auto& metadata = data->reader()->metadata();
    const auto& map = metadata.map();

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setRowCount(map.size());

    int numberLines=0;
    for (auto element : map) // Only int, double and string metadata are displayed.
    {
        QTableWidgetItem* col0=new QTableWidgetItem();
        QTableWidgetItem* col1=new QTableWidgetItem();
        col0->setData(Qt::EditRole,QString(element.first));

        if (element.second.is<int>())
        {
            col1->setData(Qt::EditRole,element.second.as<int>());
        }
        else if (element.second.is<double>())
        {
            col1->setData(Qt::EditRole,element.second.as<double>());
        }
        else if (element.second.is<std::string>())
        {
            col1->setData(Qt::EditRole,QString(QString::fromStdString(element.second.as<std::string>())));
        }
        else
        {
            delete col0;
            delete col1;
            continue;
        }
        ui->tableWidget->setItem(numberLines,0,col0);
        ui->tableWidget->setItem(numberLines++,1,col1);
    }
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

NumorPropertyWidget::~NumorPropertyWidget()
{
    delete ui;
}
