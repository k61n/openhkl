#include "SampleShapeItem.h"
#include "SampleShapePropertyWidget.h"

SampleShapeItem::SampleShapeItem() : InspectableTreeItem() {
  QIcon icon(":/resources/sampleIcon.png");
  setIcon(icon);

  setText("Shape");

  setEditable(false);

  setDragEnabled(false);
  setDropEnabled(false);

  setSelectable(false);

  setCheckable(false);
}

QWidget *SampleShapeItem::inspectItem() {
  return new SampleShapePropertyWidget(this);
}
