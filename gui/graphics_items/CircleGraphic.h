//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/CircleGraphic.h
//! @brief     Defines class PeakItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_ITEMS_CIRCLEGRAPHIC_H
#define OHKL_GUI_GRAPHICS_ITEMS_CIRCLEGRAPHIC_H

#include <Eigen/Dense>
#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPen>

//! Plottable graphics item allowing manualrepositioning of direct beam
class CircleGraphic : public QGraphicsItem {
 public:
    CircleGraphic(const QPointF& centre, int radius);

    QRectF boundingRect() const override; //!< Overloading boundingRect from QGraphicsItem

    //! Redraw all elements
    void redraw();
    //! Required to avoid abstract class
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    //! Get the radius of the circle
    int radius() { return _radius; };
    //! Set the radius of the circle
    void setRadius(int radius);
    //! Get the linewidth
    int linewidth() { return _linewidth; };
    //! Set the linewidth of the circle
    void setLinewidth(int width);
    //! Show/hide the circle
    void show(bool s);
    //! Set the text
    void setLabel(const QString& text);

 private:
    QGraphicsEllipseItem* _circle;
    QGraphicsEllipseItem* _peak;
    QGraphicsTextItem* _label;
    QPen _pen;
    int _radius;
    int _peak_radius;
    int _linewidth;
};

#endif // OHKL_GUI_GRAPHICS_ITEMS_CIRCLEGRAPHIC_H
