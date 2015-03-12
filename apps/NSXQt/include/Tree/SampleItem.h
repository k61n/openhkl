#ifndef SAMPLEITEM_H
#define SAMPLEITEM_H

#include <string>
#include "InspectableTreeItem.h"

using namespace SX::Instrument;

class SampleItem : public InspectableTreeItem
{
public:
    explicit SampleItem(Experiment* experiment);

    void setData(const QVariant & value, int role=Qt::UserRole + 1 );
    QWidget* inspectItem();
};

#endif // SAMPLEITEM_H
