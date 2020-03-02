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
    //! return the actual peak pointer
    nsx::Peak3D* peak() const;
    //! Show the labels
    void showLabel(bool flag);
    //! Show the peak area
    void showArea(bool flag);
    //! Change the peak size
    void setSize(int size);
    //! Change the peak color
    void setColor(QColor color);
    void setOutlineColor(QColor color);

 private:
    //! Pointer to the Peak3D object
    nsx::Peak3D* _peak;
    //! Flag to show the label
    bool _show_label;
    //! Flag to show the area
    bool _show_center;
    //! The area item
    QGraphicsEllipseItem* _center_gi;
    //! Lower boundaries
    Eigen::Vector3d _lower;
    //! Upper boundaries
    Eigen::Vector3d _upper;
    //! Size
    Eigen::Vector2d _size;
    //! Color
    QColor _color;
};

#endif // GUI_GRAPHICS_ITEMS_PEAKITEM_H
