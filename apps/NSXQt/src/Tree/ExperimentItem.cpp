#include <QIcon>

#include "Tree/ExperimentItem.h"

ExperimentItem::ExperimentItem(Experiment* experiment) : TreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getName()));
    setForeground(QBrush(QColor("blue")));
    QIcon icon(":/resources/experimentIcon.png");
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
    setEditable(true);
}

void ExperimentItem::setData(const QVariant &value, int role)
{
    QStandardItem::setData(value,role);
    _experiment->setName(text().toStdString());
}

ExperimentItem::~ExperimentItem()
{
    if (_experiment)
        delete _experiment;
}
