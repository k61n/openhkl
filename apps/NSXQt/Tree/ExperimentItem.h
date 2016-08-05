#ifndef EXPERIMENTITEM_H
#define EXPERIMENTITEM_H

#include <string>

#include "TreeItem.h"
#include "Experiment.h"

using namespace SX::Instrument;

class ExperimentItem : public TreeItem
{
public:
    explicit ExperimentItem(std::shared_ptr<Experiment> experiment);

    void setData(const QVariant & value, int role=Qt::UserRole + 1 );

    ~ExperimentItem();

signals:

};

#endif // EXPERIMENTITEM_H
