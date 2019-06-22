//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/SXGraphicsItem.h
//! @brief     Defines class SXGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>

class DetectorScene;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsTextItem;
class QGraphicsSceneWheelEvent;
class QKeyEvent;
class QWidget;

class SXGraphicsItem : public QGraphicsItem {
 public:
    // Construct a SX graphics item
    SXGraphicsItem(QGraphicsItem* parent = 0, bool deletable = false, bool movable = false);
    // Destructor
    virtual ~SXGraphicsItem();

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    virtual void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    virtual QRectF boundingRect() const = 0;
    DetectorScene* getScene() const;
    //! Set whether or not the item is deletable
    void setDeletable(bool deletable);
    //! Set whether or not the item is movable
    void setMovable(bool movable);

    //! Returns whether or not the item is deletable
    bool isDeletable() const;
    //! Return whether or not the item at the given position is fully inside the
    //! scene
    virtual bool isInScene(const QPointF& pos) const;
    //! Returns whether or not the item is movable
    bool isMovable() const;
    //! Paint the graphics item
    virtual void
    paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) = 0;
    //! Show or does not show the label bound to the item
    void showLabel(bool);

 protected:
    bool _deletable;
    bool _hovered;
    bool _movable;
    bool _firstMove;
    QPointF _lastPos;
    QPen _pen;

    //! Text child object that contains hkl label
    QGraphicsTextItem* _label_gi;
};
