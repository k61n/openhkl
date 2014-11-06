#include "TreeItem.h"

#include <iostream>

#include <QStandardItem>
#include <QString>

TreeItem::TreeItem(Experiment* experiment) : QStandardItem(), _experiment(experiment)
{
}

TreeItem::~TreeItem()
{
    std::cout<<"I DELETE THE ITEM"<<std::endl;
}

Experiment* TreeItem::getExperiment()
{
    return _experiment;
}
