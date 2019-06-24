//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/SXGraphicsItem.h
//! @brief     Defines class NSXGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_SXGRAPHICSITEM_H
#define GUI_GRAPHICS_SXGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPen>

class DetectorScene;
class NSXGraphicsItem : public QGraphicsItem {
 public:
    // Construct a SX graphics item
    NSXGraphicsItem(QGraphicsItem* parent = 0, bool deletable = false, bool movable = false);
    // Destructor
    virtual ~NSXGraphicsItem();

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    virtual void wheelEvent(QGraphicsSceneWheelEvent* event) override;

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
    //! Show or do not show the label bound to the item
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

#endif // GUI_GRAPHICS_SXGRAPHICSITEM_H
