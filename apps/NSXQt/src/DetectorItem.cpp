#include "DetectorItem.h"

#include <QIcon>
#include <QString>

#include "Detector.h"
#include "Diffractometer.h"

DetectorItem::DetectorItem(Experiment* experiment) : TreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getDiffractometer()->getDetector()->getName()));

    QIcon icon(":/resources/detectorIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);
}
