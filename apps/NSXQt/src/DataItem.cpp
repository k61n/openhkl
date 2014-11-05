#include "DataItem.h"

#include <QIcon>
#include <QString>

DataItem::DataItem(const std::string& name) : QStandardItem()
{
    setEditable(false);
    QIcon icon(":/resources/dataIcon.png");
    setText(QString::fromStdString(name));
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
}
