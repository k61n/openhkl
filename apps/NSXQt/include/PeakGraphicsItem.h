#ifndef PEAKGRAPHICSITEM_H
#define PEAKGRAPHICSITEM_H

#include <string>

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

    void plot(SXCustomPlot* plot);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setFrame(int);

    //! Returns the type of plot related to the item
    std::string getPlotType() const;

    //! Return the peak object
    SX::Crystal::Peak3D* getPeak();
    static void setLabelVisible(bool flag=true);
private:

//    //! Turn _hoverOn true
//    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

//    //! Turn _hoverOn false
//    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    //! Pointer to the Peak3D object
    SX::Crystal::Peak3D* _peak;
    //! (h,k,l) index visible in GraphicsScene
    static bool _labelVisible;

};

#endif // PEAKGRAPHICSITEM_H
