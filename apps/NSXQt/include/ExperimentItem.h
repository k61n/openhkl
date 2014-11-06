#ifndef EXPERIMENTITEM_H
#define EXPERIMENTITEM_H

#include <string>

#include "TreeItem.h"
#include "Experiment.h"

using namespace SX::Instrument;

class ExperimentItem : public TreeItem
{
public:
    explicit ExperimentItem(Experiment* experiment);

    void setData(const QVariant & value, int role=Qt::UserRole + 1 );

signals:

};

#endif // EXPERIMENTITEM_H
