#ifndef SLICEGRAPHICSITEM_H
#define SLICEGRAPHICSITEM_H

#include <QPen>
#include <CutterGraphicsItem.h>

namespace SX
{
    namespace Data
    {
        class IData;
    }
}

class SliceGraphicsItem : public CutterGraphicsItem
{
public:
    explicit SliceGraphicsItem(SX::Data::IData*, bool horizontal=true);
    //! Paint the slice
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
private:
    SX::Data::IData* _data;
    //! Horizontal or vertical integration
    bool _horizontal;
};

#endif // SLICEGRAPHICSITEM_H
