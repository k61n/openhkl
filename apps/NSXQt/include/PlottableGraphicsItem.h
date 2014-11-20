#ifndef PLOTTABLEGRAPHICSITEM_H
#define PLOTTABLEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsTextItem;
class QWidget;
class SXCustomPlot;

class PlottableGraphicsItem : public QGraphicsItem
{
public:

    PlottableGraphicsItem(QGraphicsItem *parent=0);
    virtual ~PlottableGraphicsItem();

    virtual QRectF boundingRect() const=0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)=0;
    virtual SXCustomPlot* createPlot(QWidget* parent=0)=0;

    // Events

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

    // Getters and setters

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
    //! Show or does not show the label bound to the item
    void showLabel(bool);

protected:
    bool _deletable;
    bool _hover;
    bool _movable;
    QPen _pen;

    //! Text child object that contains hkl label
    QGraphicsTextItem* _label;

};

#endif // PLOTTABLEGRAPHICSITEM_H
