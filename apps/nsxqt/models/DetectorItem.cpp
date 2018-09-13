#include <QIcon>

#include "DetectorItem.h"
#include "DetectorPropertyWidget.h"

DetectorItem::DetectorItem() : InspectableTreeItem()
{
    setText("Detector");

    QIcon icon(":/resources/detectorIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(false);
}

QWidget* DetectorItem::inspectItem()
{
    return new DetectorPropertyWidget(this);
}
