#include <QWidget>

#include <nsxlib/data/DataSet.h>
#include <nsxlib/data/MetaData.h>

#include "models/NumorItem.h"
#include "tree/NumorPropertyWidget.h"

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

    auto data=caller->getData();
    if (!data)
        return;

    ui->label_Data->setText(QString::fromStdString(data->getBasename()));

    auto metadata=data->getMetadata();
    const auto& map=metadata->getMap();

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setRowCount(map.size());

    int numberLines=0;
    for (auto element : map) // Only int, double and string metadata are displayed.
    {
        QTableWidgetItem* col0=new QTableWidgetItem();
        QTableWidgetItem* col1=new QTableWidgetItem();
        col0->setData(Qt::EditRole,QString(element.first));

        if (typeid(int)==element.second.type())
        {
            col1->setData(Qt::EditRole,int(boost::any_cast<int>(element.second)));
        }
        else if (typeid(double)==element.second.type())
        {
            col1->setData(Qt::EditRole,double(boost::any_cast<double>(element.second)));
        }
        else if (typeid(std::string)==element.second.type())
        {
            col1->setData(Qt::EditRole,QString(QString::fromStdString(boost::any_cast<std::string>(element.second))));
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
