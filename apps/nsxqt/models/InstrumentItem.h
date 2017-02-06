#ifndef INSTRUMENTITEM_H
#define INSTRUMENTITEM_H

#include <string>

#include <nsxlib/instrument/Experiment.h>
#include "models/TreeItem.h"

class DetectorItem;
class SampleItem;
class SourceItem;

class InstrumentItem : public TreeItem
{
public:
    explicit InstrumentItem(std::shared_ptr<Experiment> experiment);

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;

public:

    DetectorItem* getDetectorItem();
    SampleItem* getSampleItem();
    SourceItem* getSourceItem();

private:
    DetectorItem* _detector;
    SampleItem* _sample;
    SourceItem* _source;
};

#endif // INSTRUMENTITEM_H
