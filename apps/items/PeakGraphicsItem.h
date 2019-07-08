//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/PeakGraphicsItem.h
//! @brief     Defines class PeakGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QPointF>

#include <string>

#include "core/peak/Peak3D.h"
#include "core/shape/ShapeLibrary.h"

#include "apps/items/PlottableGraphicsItem.h"

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

};
