#include <QIcon>

#include <core/Diffractometer.h>
#include <core/Experiment.h>
#include <core/Source.h>

#include "SourceItem.h"
#include "SourcePropertyWidget.h"

SourceItem::SourceItem(const char *name) : InspectableTreeItem() {
  setText(name);

  QIcon icon(":/resources/sourceIcon.png");
  setIcon(icon);

  setEditable(false);

  setDragEnabled(false);
  setDropEnabled(false);

  setSelectable(false);

  setCheckable(false);
}

QWidget *SourceItem::inspectItem() { return new SourcePropertyWidget(this); }
