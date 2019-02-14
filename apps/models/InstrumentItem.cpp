#include <QIcon>
#include <QString>

#include <core/Diffractometer.h>
#include <core/Source.h>

#include "DetectorItem.h"
#include "InstrumentItem.h"
#include "SampleItem.h"
#include "SourceItem.h"

InstrumentItem::InstrumentItem(const char *name, const char *source_name)
    : TreeItem() {
  setText(name);

  QIcon icon(":/resources/instrumentIcon.png");
  setIcon(icon);

  setEditable(false);

  setDragEnabled(true);
  setDropEnabled(true);

  setSelectable(false);

  setCheckable(false);

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

DetectorItem *InstrumentItem::detectorItem() { return _detector; }

SampleItem *InstrumentItem::sampleItem() { return _sample; }

SourceItem *InstrumentItem::sourceItem() { return _source; }
