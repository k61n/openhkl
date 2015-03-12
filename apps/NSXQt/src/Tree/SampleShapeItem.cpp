#include <QWidget>

#include "Tree/SampleShapeItem.h"
#include "Tree/SampleShapePropertyWidget.h"

SampleShapeItem::SampleShapeItem(SX::Instrument::Experiment* experiment):
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

SampleShapeItem::~SampleShapeItem()
{

}

QWidget* SampleShapeItem::inspectItem()
{
    return new SampleShapePropertyWidget(this);
}

