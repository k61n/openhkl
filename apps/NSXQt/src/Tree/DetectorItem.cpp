#include <QIcon>
#include <QString>

#include "Detector.h"
#include "Diffractometer.h"

#include "Tree/DetectorPropertyWidget.h"
#include "Tree/DetectorItem.h"

DetectorItem::DetectorItem(Experiment* experiment) : InspectableTreeItem(experiment)
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
        return new DetectorPropertyWidget();
}
