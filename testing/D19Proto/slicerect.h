#ifndef SLICERECT_H
#define SLICERECT_H

#include <QGraphicsRectItem>

class SliceRect : public QGraphicsRectItem
{
public:
    SliceRect(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0);
};

#endif // SLICERECT_H
