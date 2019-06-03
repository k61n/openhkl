#ifndef GUI_GRAPHICS_PLOTTABLEITEM_H
#define GUI_GRAPHICS_PLOTTABLEITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>

#include "gui/graphics/nsxgraphicsitem.h"

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsTextItem;
class QGraphicsSceneWheelEvent;
class QKeyEvent;
class QWidget;
class NSXPlot;

class PlottableItem : public NSXGraphicsItem {
public:
    PlottableItem(QGraphicsItem* parent = 0, bool deletable = true, bool movable = false);
    virtual ~PlottableItem();

    virtual void plot(NSXPlot* plot) = 0;

    // Getters and setters

    //! Returns the type of plot related to the item
    virtual std::string getPlotType() const = 0;

    // Other methods

    bool isPlottable(NSXPlot* plot) const;
};

#endif //GUI_GRAPHICS_PLOTTABLEITEM_H
