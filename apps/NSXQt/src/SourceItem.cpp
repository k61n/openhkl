#include "SourceItem.h"

#include <QIcon>
#include <QString>

SourceItem::SourceItem(const std::string& name) : QStandardItem()
{
    setEditable(false);
    QIcon icon(":/resources/sourceIcon.png");
    setIcon(icon);
    setText(QString::fromStdString(name));
    setDragEnabled(true);
    setDropEnabled(true);
}
