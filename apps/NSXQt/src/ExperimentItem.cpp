#include "ExperimentItem.h"
#include <iostream>

#include <QIcon>

ExperimentItem::ExperimentItem(Experiment* experiment) : TreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getName()));

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
    std::cout<<"I DELETE THE EXEPERIMENT ITEM"<<std::endl;
    if (_experiment)
        delete _experiment;
}
