#include <QIcon>
#include <QString>

#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/Diffractometer.h>

#include "tree/DetectorPropertyWidget.h"
#include "models/DetectorItem.h"

DetectorItem::DetectorItem(std::shared_ptr<Experiment> experiment) : InspectableTreeItem(experiment)
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

    // NOT IMPLEMENTED

    return obj;
}
