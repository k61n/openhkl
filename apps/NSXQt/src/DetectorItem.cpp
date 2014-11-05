#include "DetectorItem.h"

#include <QIcon>
#include <QString>

DetectorItem::DetectorItem(const std::string& name) : QStandardItem()
{
    setEditable(false);
    QIcon icon(":/resources/detectorIcon.png");
    setText(QString::fromStdString(name));
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
}
