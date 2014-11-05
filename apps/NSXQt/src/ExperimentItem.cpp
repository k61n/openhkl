#include "ExperimentItem.h"

#include <QIcon>
#include <QString>

ExperimentItem::ExperimentItem(const std::string& name) : QStandardItem()
{
    setEditable(false);
    QIcon icon(":/resources/experimentIcon.png");
    setText(QString::fromStdString(name));
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
}

void ExperimentItem::appendRow(QStandardItem *item)
{
    QStandardItem::appendRow(item);
}
