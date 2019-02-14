#pragma once

#include <QGraphicsRectItem>

class SliceRectGraphicsItem : public QGraphicsRectItem {
public:
  SliceRectGraphicsItem(qreal x, qreal y, qreal width, qreal height,
                        QGraphicsItem *parent = 0);
};
