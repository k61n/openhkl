//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/EllipseItem.h
//! @brief     Defines class EllipseMaskItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_ITEMS_ELLIPSEMASKITEM_H
#define OHKL_GUI_GRAPHICS_ITEMS_ELLIPSEMASKITEM_H

#include "base/geometry/AABB.h"
#include "core/data/DataTypes.h"
#include "gui/graphics_items/SXGraphicsItem.h"

//! Creates an ellipse mask

//! Creates a mask that will be used to unselect/select peaks whether their intercept or
//! not the mask
class EllipseMaskItem : public SXGraphicsItem {
 public:
    // Constructs a mask
    EllipseMaskItem(ohkl::sptrDataSet data, ohkl::AABB* aabb);
    //! The destructor
    ~EllipseMaskItem();

    // Events

    //! The mouse move event.
    //! If the item is selected when the event is triggered then the item will be
    //! moved on the scene Otherwise, that means that the item is being drawn and
    //! then the move event corresponds to a resize
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    // Getters and setters

    //! Returns the bounding rectangle of the mask
    QRectF boundingRect() const override;
    ohkl::AABB* getAABB();
    //! Sets the starting corner of the mask
    void setFrom(const QPointF& pos);
    //! Overload setFrom for an Eigen::Vector3d argument
    void setFrom(const Eigen::Vector3d& vec);
    //! Sets the ending corner of the mask
    void setTo(const QPointF& pos);
    //! Overload setTo for an Eigen::Vector3d argument
    void setTo(const Eigen::Vector3d& vec);

    // Other methods

    //! Paint the slice
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 protected:
    //! The data on which the cutter will act upon
    ohkl::sptrDataSet _data;
    //! The AABB of the peak
    ohkl::AABB* _aabb;
    QPointF _from;
    QPointF _to;
    QGraphicsTextItem* _text;

 private:
    void updateAABB();
};

#endif // OHKL_GUI_GRAPHICS_ITEMS_ELLIPSEITEM_H
