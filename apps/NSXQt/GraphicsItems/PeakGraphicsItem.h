#ifndef PEAKGRAPHICSITEM_H
#define PEAKGRAPHICSITEM_H

#include <QPointF>

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
class SXPlot;

class PeakGraphicsItem : public PlottableGraphicsItem
{
public:

    PeakGraphicsItem(SX::Crystal::Peak3D* p);

    ~PeakGraphicsItem();

    void plot(SXPlot* plot);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setFrame(int);

    //! Returns the type of plot related to the item
    std::string getPlotType() const;

    //! Return the peak object
    SX::Crystal::Peak3D* getPeak();

    static void setLabelVisible(bool flag=true);

    void calculatePoints(int frame);

private:

    //! Pointer to the Peak3D object
    SX::Crystal::Peak3D* _peak;
    //! (h,k,l) index visible in GraphicsScene
    static bool _labelVisible;

    std::vector<QPointF> _peakPoints;
    std::vector<QPointF> _bkgPoints;

};

#endif // PEAKGRAPHICSITEM_H
