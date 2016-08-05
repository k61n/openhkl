#include <QIcon>

#include "Tree/ExperimentItem.h"

ExperimentItem::ExperimentItem(std::shared_ptr<Experiment> experiment) : TreeItem(experiment)
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
    // no longer needed since _experiment is a smart pointer
    //if (_experiment)
    //    delete _experiment;
}
