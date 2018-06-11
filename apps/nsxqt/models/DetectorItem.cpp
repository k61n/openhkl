#include <QIcon>

#include "DetectorItem.h"
#include "DetectorPropertyWidget.h"

DetectorItem::DetectorItem() : InspectableTreeItem()
{
    setText("Detector");
    QIcon icon(":/resources/detectorIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);
}

QWidget* DetectorItem::inspectItem()
{
    return new DetectorPropertyWidget(this);
}
