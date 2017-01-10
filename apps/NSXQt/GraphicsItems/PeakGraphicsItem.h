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
using sptrPeak3D = std::shared_ptr<Peak3D>;
}
}

class QWidget;
class SXPlot;

using SX::Crystal::sptrPeak3D;

class PeakGraphicsItem : public PlottableGraphicsItem
{
public:

    PeakGraphicsItem(sptrPeak3D p);

    ~PeakGraphicsItem();

    void plot(SXPlot* plot);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setFrame(unsigned long);

    //! Returns the type of plot related to the item
    std::string getPlotType() const;

    //! Return the peak object
    sptrPeak3D getPeak();

    static void setLabelVisible(bool flag=true);
    static void drawBackground(bool flag);

private:
    struct Ellipse {
        double a, b, u, v, alpha;
    };

    static Ellipse calculateEllipse(const SX::Geometry::IShape<double, 3>& shape, int frame);
    static void drawEllipse(QPainter& painter, Ellipse ellipse);

    //! Pointer to the Peak3D object
    sptrPeak3D _peak;
    //! (h,k,l) index visible in GraphicsScene
    static bool _labelVisible;
    static bool _drawBackground;

    Ellipse _peakEllipse;
    Ellipse _bkgEllipse;
};

#endif // PEAKGRAPHICSITEM_H
