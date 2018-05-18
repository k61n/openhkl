#include <QIcon>

#include "PeakListItem.h"
#include "PeakListPropertyWidget.h"

PeakListItem::PeakListItem() : InspectableTreeItem()
{
	setText("Peaks");
    setEditable(true);
    setDragEnabled(true);
    setDropEnabled(true);
    setSelectable(true);
    setCheckable(true);
}

QWidget* PeakListItem::inspectItem()
{
    return new PeakListPropertyWidget(this);
}
