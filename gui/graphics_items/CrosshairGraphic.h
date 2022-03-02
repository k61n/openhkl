//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/CrosshairGraphic.h
//! @brief     Defines class PeakItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_ITEMS_CROSSHAIRGRAPHIC_H
#define NSX_GUI_GRAPHICS_ITEMS_CROSSHAIRGRAPHIC_H

#include <Eigen/Dense>
#include <QColor>
#include <QGraphicsItem>
#include <QPen>

//! Plottable graphics item allowing manualrepositioning of direct beam
class CrosshairGraphic : public QGraphicsItem {
 public:
    CrosshairGraphic(QPointF centre);

    QRectF boundingRect() const override; //!< Overloading boundingRect from QGraphicsItem

    //! Redraw all elements
    void redraw();
    //! Required to avoid abstract class
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    //! Set the size of the crosshair
    void setSize(int size);
    //! Set the linewidth of the crosshair
    void setLinewidth(int width);
    //! Show/hide the crosshair
    void show(bool s);

 private:
    QGraphicsPathItem* _xhair;
    QPen _pen;
    int _size;
    int _linewidth;
};

#endif // NSX_GUI_GRAPHICS_ITEMS_CROSSHAIRGRAPHIC_H
