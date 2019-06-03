
#include "gui/graphics/plottableitem.h"
#include "gui/graphics/nsxplot.h"

PlottableItem::PlottableItem(QGraphicsItem* parent, bool deletable, bool movable)
    : NSXGraphicsItem(parent, deletable, movable)
{
}

PlottableItem::~PlottableItem() {}

bool PlottableItem::isPlottable(NSXPlot* plot) const
{
    if (!plot)
        return false;
    else
        return (getPlotType().compare(plot->getType()) == 0);
}
