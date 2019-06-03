
#ifndef GUI_GRAPHICS_CUTSLICEITEM_H
#define GUI_GRAPHICS_CUTSLICEITEM_H

#include "gui/graphics/cutteritem.h"

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class NSXPlot;

class CutSliceItem : public CutterItem {
public:
    // Constructors and destructor

    //! Construct a data slicer
    CutSliceItem(nsx::sptrDataSet, bool horizontal = true);
    //! The destructor
    ~CutSliceItem();

    // Events

    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event);

    // Getters and setters

    //! Returns the type of plot related to this item
    std::string getPlotType() const;

    // Other methods

    //! Plot the item
    void plot(NSXPlot*);
    //! Return true if the slice is horizontal.
    bool isHorizontal() const;
    //! Paint the slice
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    //! Horizontal or vertical integration
    bool _horizontal;
};

#endif //GUI_GRAPHICS_CUTSLICEITEM_H
