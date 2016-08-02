#include <QStandardItem>
#include <QString>

#include "Tree/TreeItem.h"

TreeItem::TreeItem(std::shared_ptr<Experiment> experiment) : QStandardItem(), _experiment(experiment)
{
}

TreeItem::~TreeItem()
{
}

std::shared_ptr<Experiment> TreeItem::getExperiment()
{
    return _experiment;
}
