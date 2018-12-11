#include "SampleItem.h"

#include <algorithm>
#include <QIcon>

#include "FrameSampleGlobalOffsets.h"
#include "SamplePropertyWidget.h"
#include "SampleShapeItem.h"

SampleItem::SampleItem() : InspectableTreeItem()
{
    setText("Sample");

    QIcon icon(":/resources/gonioIcon.png");

    setIcon(icon);

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(false);

    SampleShapeItem* shape = new SampleShapeItem();
    appendRow(shape);
}

QWidget* SampleItem::inspectItem()
{
    return new SamplePropertyWidget(experiment()->diffractometer()->sample());
}

void SampleItem::openSampleGlobalOffsetsFrame()
{
    FrameSampleGlobalOffsets *frame = FrameSampleGlobalOffsets::create(experimentItem());

    frame->show();
    frame->raise();
}
