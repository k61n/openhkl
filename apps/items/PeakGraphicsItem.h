#pragma once

#include <QPointF>

#include <string>

#include <core/CrystalTypes.h>
#include <core/GeometryTypes.h>

#include "PlottableGraphicsItem.h"

class QWidget;
class SXPlot;

class PeakGraphicsItem : public PlottableGraphicsItem {
public:
    PeakGraphicsItem(nsx::sptrPeak3D peak, int frame);

    ~PeakGraphicsItem() = default;

    void plot(SXPlot* plot);

    QRectF boundingRect() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    //! Returns the type of plot related to the item
    std::string getPlotType() const;

    nsx::sptrPeak3D peak() const;

    static void showLabel(bool flag);

    static void showArea(bool flag);

private:
    //! Pointer to the Peak3D object
    nsx::sptrPeak3D _peak;

    static bool _show_label;

    static bool _show_center;

    QGraphicsEllipseItem* _center_gi;

    Eigen::Vector3d _lower;

    Eigen::Vector3d _upper;

    QGraphicsRectItem* _area;
};
