//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/PinItem.h
//! @brief     Defines class PinItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_ITEMS_PINITEM_H
#define NSX_GUI_GRAPHICS_ITEMS_PINITEM_H

#include <QGraphicsItem>

//!
/* @class PinItem: Class used to calibrate
  the position of the crystal pin as function
  of the rotation angle.

  */

class PinItem : public QGraphicsItem {
 public:
    PinItem(QGraphicsItem* parent = 0);
    void initiate(double posy, double angle);
    //! Method called when point is moved in another frame
    void adjust(double posy, double angle);
    //! Overloaded
    QRectF boundingRect() const;
    //! Paint method
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    //! Methiod called when the frame is rotated to a new angle
    void rotate(double angle);
    bool iscalibrated() const;
    void getCenter(double& y, double& z, double angle) const;

 private:
    double _p1y, _p2y;
    double _angle;
    double _currentangle;
    bool _calibrated;
};

#endif // NSX_GUI_GRAPHICS_ITEMS_PINITEM_H
