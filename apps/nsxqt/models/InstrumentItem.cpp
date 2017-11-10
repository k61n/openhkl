#include <QIcon>
#include <QString>

#include "DetectorItem.h"
#include "InstrumentItem.h"
#include "SampleItem.h"
#include "SourceItem.h"

InstrumentItem::InstrumentItem(nsx::sptrExperiment experiment) : TreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getDiffractometerType()));

    QIcon icon(":/resources/instrumentIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);

    // Create a detector item and add it to the instrument item
    _detector = new DetectorItem(experiment);
    appendRow(_detector);

    // Create a sample item and add it to the instrument item
    _sample = new SampleItem(experiment);
    appendRow(_sample);

    // Create a source item and add it to the instrument leaf
    _source = new SourceItem(experiment);
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

DetectorItem* InstrumentItem::getDetectorItem()
{
    return _detector;
}

SampleItem* InstrumentItem::getSampleItem()
{
    return _sample;
}

SourceItem* InstrumentItem::getSourceItem()
{
    return _source;
}
