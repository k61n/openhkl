#include "SampleItem.h"

#include <QIcon>
#include <QString>

SampleItem::SampleItem(const std::string& name) : QStandardItem()
{
    setEditable(false);
    QIcon icon(":/sampleIcon.png");
    setText(QString::fromStdString(name));
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
}
