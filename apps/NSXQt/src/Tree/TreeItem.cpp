#include <QStandardItem>
#include <QString>

#include "Tree/TreeItem.h"

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
