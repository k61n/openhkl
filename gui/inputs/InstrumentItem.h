#pragma once

#include <QJsonObject>

#include "TreeItem.h"

class DetectorItem;
class SampleItem;
class SourceItem;

class InstrumentItem : public TreeItem {
public:
    explicit InstrumentItem(const char* name, const char* source_name);

public:
    DetectorItem* detectorItem();
    SampleItem* sampleItem();
    SourceItem* sourceItem();

private:
    DetectorItem* _detector;
    SampleItem* _sample;
    SourceItem* _source;
};
