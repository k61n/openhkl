#ifndef NSXQT_SLICERECTGRAPHICSITEM_H
#define NSXQT_SLICERECTGRAPHICSITEM_H

#include <QGraphicsRectItem>

class SliceRectGraphicsItem : public QGraphicsRectItem
{
public:
    SliceRectGraphicsItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0);
};

#endif // NSXQT_SLICERECTGRAPHICSITEM_H
