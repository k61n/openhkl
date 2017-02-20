#include <QCursor>
#include <QtDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include "DetectorScene.h"
#include "PlottableGraphicsItem.h"
#include "plot/SXPlot.h"

PlottableGraphicsItem::PlottableGraphicsItem(QGraphicsItem *parent, bool deletable, bool movable)
: SXGraphicsItem(parent, deletable, movable)
{
}

PlottableGraphicsItem::~PlottableGraphicsItem()
{
}

bool PlottableGraphicsItem::isPlottable(SXPlot* plot) const
{
    if (!plot)
        return false;
    else
        return (getPlotType().compare(plot->getType())==0);
}
