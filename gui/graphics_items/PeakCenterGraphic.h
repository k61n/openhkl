//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/PeakCenterGraphic.h
//! @brief     Defines class PeakItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_ITEMS_PEAKCENTERGRAPHIC_H
#define NSX_GUI_GRAPHICS_ITEMS_PEAKCENTERGRAPHIC_H

#include "gui/graphics_items/PlottableItem.h"

#include <Eigen/Dense>
#include <QColor>

//! Plottable graphics item that represents a peak centre from a 3rd party code in the detector image
class PeakCenterGraphic : public QGraphicsItem {
 public:
    PeakCenterGraphic(Eigen::Vector3d peak);
    ~PeakCenterGraphic() = default;

    //! Redraw all the elements of the item
    void redraw();
    //! Set the center of the element
    void setCenter(Eigen::Vector3d center);

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //! Show the labels
    void showLabel(bool flag);
    //! Show the peak
    void showArea(bool flag);
    //! Show the peak area
    void setSize(int size);
    //! Change the peak color
    void setColor(QColor color);

 private:
    //! The plottable item
    QGraphicsEllipseItem* _center_gi;
    //! The peak label
    QGraphicsTextItem* _label_gi;
    //! Flag to show the label
    bool _show_label;
    //! Flag to show the area
    bool _show_center;
    //! Size
    Eigen::Vector2d _size;
    //! Peak centre colour
    QColor _center_color;
};

#endif // NSX_GUI_GRAPHICS_ITEMS_PEAKCENTERGRAPHIC_H
