#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include "PlottableGraphicsItem.h"
#include "SXPlot.h"

PlottableGraphicsItem::PlottableGraphicsItem(QGraphicsItem* parent, bool deletable, bool movable)
    : SXGraphicsItem(parent, deletable, movable)
{
}

PlottableGraphicsItem::~PlottableGraphicsItem() {}

bool PlottableGraphicsItem::isPlottable(SXPlot* plot) const
{
    return plot && (getPlotType().compare(plot->getType()) == 0);
}
