#ifndef DATAITEM_H
#define DATAITEM_H

#include <string>

#include "TreeItem.h"
#include "Experiment.h"

class DataItem : public TreeItem
{
public:
    explicit DataItem(Experiment* experiment);

signals:

public slots:

};

#endif // DATAITEM_H
