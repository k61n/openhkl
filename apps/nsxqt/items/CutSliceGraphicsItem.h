#ifndef NSXQT_CUTSLICEGRAPHICSITEM_H
#define NSXQT_CUTSLICEGRAPHICSITEM_H

#include "items/CutterGraphicsItem.h"

namespace nsx
{
class DataSet;
}

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class SXPlot;

class CutSliceGraphicsItem : public CutterGraphicsItem
{
public:

    // Constructors and destructor

    //! Construct a data slicer
    CutSliceGraphicsItem(std::shared_ptr<nsx::DataSet>, bool horizontal=true);
    //! The destructor
    ~CutSliceGraphicsItem();

    // Events

    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event);

    // Getters and setters

    //! Returns the type of plot related to this item
    std::string getPlotType() const;

    // Other methods

    //! Plot the item
    void plot(SXPlot*);
    //! Return true if the slice is horizontal.
    bool isHorizontal() const;
    //! Paint the slice
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    //! Horizontal or vertical integration
    bool _horizontal;
};

#endif // NSXQT_CUTSLICEGRAPHICSITEM_H
