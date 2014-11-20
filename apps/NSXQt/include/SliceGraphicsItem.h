#ifndef SLICEGRAPHICSITEM_H
#define SLICEGRAPHICSITEM_H

#include <CutterGraphicsItem.h>

namespace SX
{
    namespace Data
    {
        class IData;
    }
}

class QGraphicsSceneWheelEvent;
class QWidget;
class SXCustomPlot;

class SliceGraphicsItem : public CutterGraphicsItem
{
public:
    SliceGraphicsItem(SX::Data::IData*, bool horizontal=true);

    ~SliceGraphicsItem();

    SXCustomPlot* createPlot(QWidget* parent=0);

    //! Paint the slice
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

    bool isHorizontal() const;

private:
    //! Horizontal or vertical integration
    bool _horizontal;
};

#endif // SLICEGRAPHICSITEM_H
