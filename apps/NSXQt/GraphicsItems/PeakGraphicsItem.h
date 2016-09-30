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
    static void drawBackground(bool flag);

private:
    struct Ellipse {
        double a, b, u, v, alpha;
    };

    static Ellipse calculateEllipse(const SX::Geometry::IShape<double, 3>& shape, int frame);
    static void drawEllipse(QPainter& painter, Ellipse ellipse);

    //! Pointer to the Peak3D object
    SX::Crystal::Peak3D* _peak;
    //! (h,k,l) index visible in GraphicsScene
    static bool _labelVisible;
    static bool _drawBackground;

    Ellipse _peakEllipse;
    Ellipse _bkgEllipse;
};

#endif // PEAKGRAPHICSITEM_H
