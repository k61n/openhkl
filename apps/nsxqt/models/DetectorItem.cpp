#include <QIcon>

#include "models/DetectorItem.h"
#include "tree/DetectorPropertyWidget.h"

DetectorItem::DetectorItem(nsx::sptrExperiment experiment) : InspectableTreeItem(experiment)
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

QJsonObject DetectorItem::toJson()
{
    QJsonObject obj;

    return obj;
}
