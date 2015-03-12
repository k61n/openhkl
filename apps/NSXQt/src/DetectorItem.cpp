#include "DetectorItem.h"
#include <QIcon>
#include <QString>
#include "Detector.h"
#include "Diffractometer.h"
#include "DetectorPropertyWidget.h"

DetectorItem::DetectorItem(Experiment* experiment) : InspectableTreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getDiffractometer()->getDetector()->getName()));

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
