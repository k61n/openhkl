#ifndef SLICEGRAPHICSITEM_H
#define SLICEGRAPHICSITEM_H

#include <string>

#include "AABB.h"

#include <CutterGraphicsItem.h>

// Forward declarations
namespace SX
{
namespace Data
{
class IData;
}
}

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

using SX::Geometry::AABB;

class MaskGraphicsItem : public SXGraphicsItem
{
public:

    // Constructors and destructor

    MaskGraphicsItem(SX::Data::IData* data);
    //! The destructor
    ~MaskGraphicsItem();

    //! Returns the bounding rectangle of the item
    QRectF boundingRect() const;
    //! Sets the top left corner of the item
    void setFrom(const QPointF& pos);
    //! Sets the bottom right corner of the item
    void setTo(const QPointF& pos);

    // Other methods

    //! Paint the slice
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void excludePeaks() const;

protected:
    //! The data on which the cutter will act upon
    SX::Data::IData* _data;

    //! The top left coordinates of the slice
    QPointF _from;
    //! The bottom right coordinates of the slice
    QPointF _to;

    AABB<double,3> _boundaries;

};

#endif // MASkGRAPHICSITEM_H
