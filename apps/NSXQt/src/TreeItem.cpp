#include "TreeItem.h"

#include <iostream>

#include <QStandardItem>
#include <QString>

TreeItem::TreeItem(Experiment* experiment) : QStandardItem(), _experiment(experiment)
{
}

TreeItem::~TreeItem()
{
}

Experiment* TreeItem::getExperiment()
{
    return _experiment;
}
