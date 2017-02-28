#ifndef CUTTERGRAPHICSITEM_H
#define CUTTERGRAPHICSITEM_H

#include <QPen>
#include <QPointF>

#include <memory>

#include "PlottableGraphicsItem.h"

namespace SX
{
namespace Data
{
class DataSet;
}
}

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QWidget;

class CutterGraphicsItem : public PlottableGraphicsItem
{
public:

    // Constructors and destructor

    //! Constructs a data cutter
    CutterGraphicsItem(std::shared_ptr<SX::Data::DataSet> data);
    //! Destructor
    virtual ~CutterGraphicsItem();

    // Events

    //! The mouse move event.
    //! If the item is selected when the event is triggered then the item will be moved on the scene
    //! Otherwise, that means that the item is being drawn and then the move event corresponds to a resize
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    // Getters and setters

    //! Returns the bounding rectangle of the item
    QRectF boundingRect() const;
    //! Returns the data bound to the item
    std::shared_ptr<SX::Data::DataSet> getData();
    //! Sets the top left corner of the item
    void setFrom(const QPointF& pos);
    //! Sets the bottom right corner of the item
    void setTo(const QPointF& pos);

    // Other methods

    //! Paint the contents of the item [pure virtual]
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)=0;


protected:
    //! The data on which the cutter will act upon
    std::shared_ptr<SX::Data::DataSet> _data;
    //! The top left coordinates of the slice
    QPointF _from;
    //! The bottom right coordinates of the slice
    QPointF _to;


};

#endif // CUTTERGRAPHICSITEM_H
