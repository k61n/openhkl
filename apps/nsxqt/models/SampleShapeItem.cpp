#include <QWidget>

#include "models/SampleShapeItem.h"
#include "tree/SampleShapePropertyWidget.h"

SampleShapeItem::SampleShapeItem(std::shared_ptr<SX::Instrument::Experiment> experiment):
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

