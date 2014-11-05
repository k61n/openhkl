#include "InstrumentItem.h"

#include <QIcon>
#include <QPixmap>
#include <QString>

InstrumentItem::InstrumentItem(const std::string& name) : QStandardItem()
{
    setEditable(false);
    QIcon icon(":/resources/instrumentIcon.png");
    setIcon(icon);
    setText(QString::fromStdString(name));
    setDragEnabled(true);
    setDropEnabled(true);
}

void InstrumentItem::appendRow(QStandardItem *item)
{
    QStandardItem::appendRow(item);
//    setText(QString::number(rowCount()));
}
