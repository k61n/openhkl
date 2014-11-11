#ifndef LINECUTGRAPHICSITEM_H
#define LINECUTGRAPHICSITEM_H

#include <QGraphicsItem>
#include <CutterGraphicsItem.h>

namespace SX
{
    namespace Data
    {
        class IData;
    }
}


class LineCutGraphicsItem : public CutterGraphicsItem
{
public:
    explicit LineCutGraphicsItem(SX::Data::IData*);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    SX::Data::IData* _data;

};

#endif // LINECUTGRAPHICSITEM_H
