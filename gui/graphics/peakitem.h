//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/peakitem.h
//! @brief     Defines class PeakItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_PEAKITEM_H
#define GUI_GRAPHICS_PEAKITEM_H

#include "core/peak/Peak3D.h"
#include "gui/graphics/plottableitem.h"

#include <Eigen/Dense>

class PeakItem : public PlottableItem {
public:
    PeakItem(nsx::sptrPeak3D peak, int frame);

    ~PeakItem() = default;

    void plot(NSXPlot* plot);

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

#endif // GUI_GRAPHICS_PEAKITEM_H
