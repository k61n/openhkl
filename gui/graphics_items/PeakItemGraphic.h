//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/PeakItem.h
//! @brief     Defines class PeakItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_ITEMS_PEAKITEM_H
#define GUI_GRAPHICS_ITEMS_PEAKITEM_H

#include "core/peak/Peak3D.h"
#include "gui/graphics_items/PlottableItem.h"
#include <Eigen/Dense>

#include <QColor>

//! Plottable graphics item that represents a peak in the detector image
class PeakItemGraphic : public PlottableItem {
 public:
    PeakItemGraphic(nsx::Peak3D* peak);
    ~PeakItemGraphic() = default;

    //! Redraw all the elements of the item
    void redraw();
    //! Set the center of the element
    void setCenter(int frame);

    void plot(SXPlot* plot) override;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //! Returns the type of plot related to the item
    std::string getPlotType() const override;

    nsx::Peak3D* peak() const;

    void showLabel(bool flag);

    void showArea(bool flag);

    void setSize(int size);

    void setColor(QColor color);

 private:
    //! Pointer to the Peak3D object
    nsx::Peak3D* _peak;

    bool _show_label;

    bool _show_center;

    QGraphicsEllipseItem* _center_gi;

    Eigen::Vector3d _lower;

    Eigen::Vector3d _upper;

    Eigen::Vector2d _size;

    QColor _color;
};

#endif // GUI_GRAPHICS_ITEMS_PEAKITEM_H
