#ifndef SAMPLEITEM_H
#define SAMPLEITEM_H

#include <string>

#include "TreeItem.h"

using namespace SX::Instrument;

class SampleItem : public TreeItem
{
public:
    explicit SampleItem(Experiment* experiment);

    void setData(const QVariant & value, int role=Qt::UserRole + 1 );

signals:

public slots:

};

#endif // SAMPLEITEM_H
