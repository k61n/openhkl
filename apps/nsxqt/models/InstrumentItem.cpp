#include <QIcon>
#include <QString>

#include <nsxlib/Diffractometer.h>
#include <nsxlib/Source.h>

#include "DetectorItem.h"
#include "InstrumentItem.h"
#include "SampleItem.h"
#include "SourceItem.h"

InstrumentItem::InstrumentItem(const char* name, const char* source_name) : TreeItem()
{
    setText(name);

    QIcon icon(":/resources/instrumentIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);

    // Create a detector item and add it to the instrument item
    _detector = new DetectorItem();
    appendRow(_detector);

    // Create a sample item and add it to the instrument item
    _sample = new SampleItem();
    appendRow(_sample);

    // Create a source item and add it to the instrument leaf
    _source = new SourceItem(source_name);
    appendRow(_source);
}

QJsonObject InstrumentItem::toJson()
{
    QJsonObject obj;

    obj["name"] = text();
    //obj["detector"] = _detector->toJson();
    obj["sample"] = _sample->toJson();
    //obj["source"] = _source->toJson();

    return obj;
}

void InstrumentItem::fromJson(const QJsonObject &obj)
{
    setText(obj["name"].toString());
    _sample->fromJson(obj["sample"].toObject());
}

DetectorItem* InstrumentItem::detectorItem()
{
    return _detector;
}

SampleItem* InstrumentItem::sampleItem()
{
    return _sample;
}

SourceItem* InstrumentItem::sourceItem()
{
    return _source;
}
