#include "NumorItem.h"

#include <QString>

NumorItem::NumorItem(const std::string& name) : QStandardItem()
{
    setEditable(false);
    setText(QString::fromStdString(name));
    setDragEnabled(true);
    setDropEnabled(true);
}
