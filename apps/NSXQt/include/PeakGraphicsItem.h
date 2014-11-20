#ifndef PEAKGRAPHICSITEM_H
#define PEAKGRAPHICSITEM_H

#include "PlottableGraphicsItem.h"

namespace SX
{
namespace Crystal
{
class Peak3D;
}
}

class QWidget;
class SXCustomPlot;

class PeakGraphicsItem : public PlottableGraphicsItem
{
public:

    PeakGraphicsItem(SX::Crystal::Peak3D* p);

    ~PeakGraphicsItem();

    SXCustomPlot* createPlot(QWidget* parent=0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setFrame(int);

    //! Return the peak object
    SX::Crystal::Peak3D* getPeak();

private:

    //! Turn _hoverOn true
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    //! Turn _hoverOn false
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    //! Pointer to the Peak3D object
    SX::Crystal::Peak3D* _peak;

};

#endif // PEAKGRAPHICSITEM_H
