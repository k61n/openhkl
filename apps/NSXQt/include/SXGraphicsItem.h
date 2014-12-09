#ifndef SXGRAPHICSITEM_H
#define SXGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>

class DetectorScene;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsTextItem;
class QGraphicsSceneWheelEvent;
class QKeyEvent;
class QWidget;

class SXGraphicsItem : public QGraphicsItem
{
public:

	// Constructors and destructor

	// Construct a SX graphics item
    SXGraphicsItem(QGraphicsItem *parent=0, bool deletable=false, bool movable=false);
    // Destructor
    virtual ~SXGraphicsItem();

    // Events

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

    // Getters and setters

    virtual QRectF boundingRect() const=0;
    DetectorScene* getScene() const;
    //! Set whether or not the item is deletable
    void setDeletable(bool deletable);
    //! Set whether or not the item is movable
    void setMovable(bool movable);

    // Other methods

    //! Returns whether or not the item is deletable
    bool isDeletable() const;
    //! Return whether or not the item at the given position is fully inside the scene
    virtual bool isInScene(const QPointF& pos) const;
    //! Returns whether or not the item is movable
    bool isMovable() const;
    //! Paint the graphics item
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)=0;
    //! Show or does not show the label bound to the item
    void showLabel(bool);

protected:
    bool _deletable;
    bool _hovered;
    bool _movable;
    QPen _pen;

    //! Text child object that contains hkl label
    QGraphicsTextItem* _label;

};

#endif // SXGRAPHICSITEM_H
