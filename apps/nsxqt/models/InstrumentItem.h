#ifndef NSXQT_INSTRUMENTITEM_H
#define NSXQT_INSTRUMENTITEM_H

#include <QJsonObject>

#include <nsxlib/InstrumentTypes.h>

#include "TreeItem.h"

class DetectorItem;
class SampleItem;
class SourceItem;

class InstrumentItem : public TreeItem
{
public:
    explicit InstrumentItem(const char* name, const char* source_name);

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

#endif // NSXQT_INSTRUMENTITEM_H
