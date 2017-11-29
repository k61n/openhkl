#ifndef NSXQT_EllipseMaskGraphicsItem_H
#define NSXQT_EllipseMaskGraphicsItem_H

#include <memory>
#include <string>

#include <nsxlib/DataTypes.h>
#include <nsxlib/GeometryTypes.h>

#include "CutterGraphicsItem.h"

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

/*! Creates a mask that will be used to unselect/select peaks whether their intercept or
 * not the mask
 */
class EllipseMaskGraphicsItem : public SXGraphicsItem {
public:
    // Constructs a mask
    EllipseMaskGraphicsItem(nsx::sptrDataSet data, nsx::AABB* aabb);
    //! The destructor
    ~EllipseMaskGraphicsItem();


    // Events

    //! The mouse move event.
    //! If the item is selected when the event is triggered then the item will be moved on the scene
    //! Otherwise, that means that the item is being drawn and then the move event corresponds to a resize
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event);

    // Getters and setters

    //! Returns the bounding rectangle of the mask
    QRectF boundingRect() const;
    nsx::AABB* getAABB();
    //! Sets the starting corner of the mask
    void setFrom(const QPointF& pos);
    //! Sets the ending corner of the mask
    void setTo(const QPointF& pos);

    // Other methods

    //! Paint the slice
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

protected:
    //! The data on which the cutter will act upon
    nsx::sptrDataSet _data;
    //! The AABB of the peak
    nsx::AABB* _aabb;
    QPointF _from;
    QPointF _to;
    QGraphicsTextItem* _text;
private:
    void updateAABB();

};

#endif // NSXQT_EllipseMaskGraphicsItem_H
