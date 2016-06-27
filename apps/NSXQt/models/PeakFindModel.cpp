#include "PeakFindModel.h"
#include <QStandardItem>
#include <QList>
#include <QDoubleSpinBox>

PeakFindModel::PeakFindModel(QObject *parent): QStandardItemModel(parent)
{
    using list = QList<QStandardItem*>;

    this->setColumnCount(2);

    // add top level items
    this->appendRow(new QStandardItem("Threshold"));
    this->appendRow(new QStandardItem("Confidence"));
    this->appendRow(new QStandardItem("Components"));
    this->appendRow(new QStandardItem("Filter"));

    // top level items are just labels, so no editable
    this->item(0, 0)->setEditable(false);
    this->item(1, 0)->setEditable(false);
    this->item(2, 0)->setEditable(false);
    this->item(3, 0)->setEditable(false);


    // properties for threshold
    QStandardItem* item = this->item(0, 0);
    item->appendRow(list() << new QStandardItem("Value") << new QStandardItem());
    item->appendRow(list() << new QStandardItem("Type") << new QStandardItem());

    item->child(0, 0)->setEditable(false);
    item->child(1, 0)->setEditable(false);

    item->child(0, 1)->setData(QVariant(3.0), Qt::DisplayRole);
    item->child(1, 1)->setData(QVariant(1.0), Qt::DisplayRole);



}

PeakFindModel::~PeakFindModel()
{

}
