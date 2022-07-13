//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/MaskItem.h
//! @brief     Defines class MaskItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_ITEMS_MASKITEM_H
#define NSX_GUI_GRAPHICS_ITEMS_MASKITEM_H

#include "base/geometry/AABB.h"
#include "core/data/DataTypes.h"
#include "gui/graphics_items/SXGraphicsItem.h"

//! Creates a mask

//! Creates a mask that will be used to unselect/select peaks whether their intercept or
//! not the mask
class MaskItem : public SXGraphicsItem {
 public:
    // Constructs a mask
    MaskItem(ohkl::sptrDataSet data, ohkl::AABB* aabb);
    //! The destructor
    ~MaskItem();

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
    //! Sets the ending corner of the mask
    void setTo(const QPointF& pos);

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

#endif // NSX_GUI_GRAPHICS_ITEMS_MASKITEM_H
