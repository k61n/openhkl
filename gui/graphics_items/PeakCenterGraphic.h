//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

//! Plottable graphics item that represents a peak centre from a 3rd party code in the detector
//! image
class PeakCenterGraphic : public QGraphicsItem {
 public:
    PeakCenterGraphic(const Eigen::Vector3d& peak);
    ~PeakCenterGraphic() = default;

    void redraw(); //!< Redraw all the elements of the item
    void setCenter(const Eigen::Vector3d& center); //!< Set the center of the element

    QRectF boundingRect() const override; //!< Overloading boundingRect from QGraphicsItem

    //! Required to avoid abstract class
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void showLabel(bool flag); //!< Show the labels
    void showArea(bool flag); //!< Show the peak
    void setSize(int size); //!< Set the size of the circle
    void setColor(QColor color); //!< Change the circle color

 private:
    QGraphicsEllipseItem* _center_gi; //!< The plottable item
    QGraphicsTextItem* _label_gi; //!< The peak centre label
    bool _show_label; //!< Flag to show the label
    bool _show_center; //!< Flag to show the peak center
    Eigen::Vector2d _size; //!< Size of circle
    QColor _center_color; //!< Circle colour
};

#endif // NSX_GUI_GRAPHICS_ITEMS_PEAKCENTERGRAPHIC_H
