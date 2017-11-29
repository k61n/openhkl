#include "SampleShapeItem.h"
#include "SampleShapePropertyWidget.h"

SampleShapeItem::SampleShapeItem(nsx::sptrExperiment experiment):
    InspectableTreeItem(experiment)
{
    QIcon icon(":/resources/sampleIcon.png");
    setIcon(icon);
    setText("Shape");
    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);
}

QWidget* SampleShapeItem::inspectItem()
{
    return new SampleShapePropertyWidget(this);
}

