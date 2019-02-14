#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>

#include "SXGraphicsItem.h"

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsTextItem;
class QGraphicsSceneWheelEvent;
class QKeyEvent;
class QWidget;
class SXPlot;

class PlottableGraphicsItem : public SXGraphicsItem
{
public:

    PlottableGraphicsItem(QGraphicsItem *parent=0, bool deletable=true, bool movable=false);
    virtual ~PlottableGraphicsItem();

    virtual void plot(SXPlot* plot)=0;

    // Getters and setters

    //! Returns the type of plot related to the item
    virtual std::string getPlotType() const=0;

    // Other methods

    bool isPlottable(SXPlot* plot) const;

};
