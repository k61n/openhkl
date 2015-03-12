#ifndef SLICERECTGRAPHICSITEM_H
#define SLICERECTGRAPHICSITEM_H

#include <QGraphicsRectItem>

class SliceRectGraphicsItem : public QGraphicsRectItem
{
public:
    SliceRectGraphicsItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0);
};

#endif // SLICERECTGRAPHICSITEM_H
