#include <algorithm>
#include <QIcon>

#include "FrameSampleGlobalOffsets.h"
#include "SampleItem.h"
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
    return new SamplePropertyWidget(this);
}

void SampleItem::openSampleGlobalOffsetsFrame()
{
    FrameSampleGlobalOffsets *frame = FrameSampleGlobalOffsets::create(experimentItem());

    frame->show();

    frame->raise();
}
