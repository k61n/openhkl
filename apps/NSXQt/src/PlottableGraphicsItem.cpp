#include <iostream>

#include <QCursor>
#include <QtDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include "DetectorScene.h"
#include "PlottableGraphicsItem.h"
#include "SXPlot.h"

PlottableGraphicsItem::PlottableGraphicsItem(QGraphicsItem *parent) : SXGraphicsItem(parent)
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
