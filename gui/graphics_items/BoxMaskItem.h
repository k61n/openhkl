//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/BoxMaskItem.h
//! @brief     Defines class BoxMaskItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_ITEMS_BOXMASKITEM_H
#define OHKL_GUI_GRAPHICS_ITEMS_BOXMASKITEM_H

#include "base/geometry/AABB.h"
#include "base/mask/BoxMask.h"
#include "core/data/DataTypes.h"
#include "gui/graphics_items/MaskItem.h"

//! Creates a mask

//! Creates a mask that will be used to unselect/select peaks whether their intercept or
//! not the mask
class BoxMaskItem : public MaskItem {
 public:
    // Constructs a graphical mask from a AABB
    BoxMaskItem(ohkl::sptrDataSet data, ohkl::AABB* aabb);
    //! The destructor
    ~BoxMaskItem();

    //! The mouse move event.
    //! If the item is selected when the event is triggered then the item will be
    //! moved on the scene Otherwise, that means that the item is being drawn and
    //! then the move event corresponds to a resize
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    //! Return a pointer to the underlying mask object
    ohkl::BoxMask* mask() const { return _mask; };
    //! Set pointer to the real mask
    void setMask(ohkl::BoxMask* mask) { _mask = mask; };
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

    //! Paint the slice
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 protected:
    //! The data on which the cutter will act upon
    ohkl::sptrDataSet _data;
    //! The actual mask
    ohkl::BoxMask* _mask;
    QPointF _from;
    QPointF _to;
    QGraphicsTextItem* _text;

 private:
    void updateAABB();
};

#endif // OHKL_GUI_GRAPHICS_ITEMS_BOXMASKITEM_H
