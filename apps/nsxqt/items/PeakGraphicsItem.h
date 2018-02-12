#ifndef NSXQT_PEAKGRAPHICSITEM_H
#define NSXQT_PEAKGRAPHICSITEM_H

#include <QPointF>

#include <string>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/GeometryTypes.h>

#include "PlottableGraphicsItem.h"

class QWidget;
class SXPlot;

class PeakGraphicsItem : public PlottableGraphicsItem
{
public:

    PeakGraphicsItem(nsx::sptrPeak3D p);
    ~PeakGraphicsItem() = default;

    void plot(SXPlot* plot);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setFrame(unsigned long);

    //! Returns the type of plot related to the item
    std::string getPlotType() const;

    //! Return the peak object
    nsx::sptrPeak3D getPeak();

    static void setLabelVisible(bool flag=true);
    static void drawBackground(bool flag);

private:

    //! Pointer to the Peak3D object
    nsx::sptrPeak3D _peak;
    //! (h,k,l) index visible in GraphicsScene
    static bool _labelVisible;
    static bool _drawBackground;

    Eigen::Vector3d _frameCenter;
    nsx::DetectorEvent _predictedCenter;
};

#endif // NSXQT_PEAKGRAPHICSITEM_H
