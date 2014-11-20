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

class QGraphicsSceneWheelEvent;
class QWidget;
class SXCustomPlot;

class LineCutGraphicsItem : public CutterGraphicsItem
{
public:

    explicit LineCutGraphicsItem(SX::Data::IData*);

    ~LineCutGraphicsItem();

    SXCustomPlot* createPlot(QWidget* parent=0);

    void wheelEvent(QGraphicsSceneWheelEvent* event);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int getNPoints() const;

    void setNPoints(int nPoints);

private:
    int _nPoints;

};

#endif // LINECUTGRAPHICSITEM_H
