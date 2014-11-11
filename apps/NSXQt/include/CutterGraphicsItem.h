#ifndef CUTTERGRAPHICSITEM_H
#define CUTTERGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPen>

class CutterGraphicsItem : public QGraphicsItem
{
public:
    CutterGraphicsItem(QGraphicsItem *parent = 0);
    //! Called when initiating drawing
    void from(qreal x, qreal y);
    //! Called when moving or finalizing drawing
    void to(qreal x, qreal z);
    QRectF boundingRect() const;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
protected:
    //! Top left coordinate of the slice
    qreal _x0,_y0;
    //! Bottom right coordinate of the slice
    qreal _x1,_y1;
    QPen _pen;

};

#endif // CUTTERGRAPHICSITEM_H
