#ifndef SLICEGRAPHICSITEM_H
#define SLICEGRAPHICSITEM_H

#include <QPen>
#include <QGraphicsItem>

namespace SX
{
    namespace Data
    {
        class IData;
    }
}

class SliceGraphicsItem : public QGraphicsItem
{
public:
    explicit SliceGraphicsItem(SX::Data::IData*, bool Horinzontal=true);
    //! Called when initiating drawing
    void from(int x, int y);
    //! Called when moving or finalizing drawing
    void to(int x, int z);
    //! Bounding region
    QRectF boundingRect() const;
    //! Paint the slice
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    SX::Data::IData* _data;
    //! Top left coordinate of the slice
    int _x0,_y0;
    //! Bottom right coordinate of the slice
    int _x1,_y1;
    //! Horizontal or vertical integration
    bool _horinzontal;
    //! Pen
    QPen _mpen;
};

#endif // SLICEGRAPHICSITEM_H
