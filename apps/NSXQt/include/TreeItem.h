#ifndef EDITABLETREEITEM_H
#define EDITABLETREEITEM_H

#include <string>

#include <QStandardItem>
#include <QVariant>

#include "Experiment.h"

using namespace SX::Instrument;

class TreeItem : public QStandardItem
{
public:
    explicit TreeItem(Experiment* experiment);
    Experiment* getExperiment();

    virtual ~TreeItem();

protected:
    Experiment* _experiment;

};

#endif // EDITABLETREEITEM_H
