//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_tools/RulerItem.h
//! @brief     Defines class RulerItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_TOOLS_RULERITEM_H
#define OHKL_GUI_GRAPHICS_TOOLS_RULERITEM_H

#include <QGraphicsItem>

class RulerItem : public QGraphicsItem {
 public:
    //! Constructor ...
    RulerItem(QGraphicsItem* parent = 0);
    ~RulerItem();
    //! Set the line thickness of the ruler
    void setLineThickness(double);
    //! Set the color of the ruler
    void setColor(const QColor&);
    //! Set the ruler width
    void setTipWidth(double);
    //! Overloaded paint method
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    //! Overloaded bounding box
    QRectF boundingRect() const;

 public:
    //! When first point is clicked, draw
    void startDrawingAt(const QPointF&);
    //! when ruler end is dragged
    void moveTipAt(const QPointF&);
    //! Stamp the ruler corresponding to a calibrated distance
    QGraphicsItemGroup* stampRuler(double distance, const QPointF& pos);
    void activateDistanceDisplay();
    void setDistance(double val);
    double getConversion() const;

 private:
    //! The base of the ruler
    QGraphicsLineItem* _base;
    //! The tip of the ruler
    QGraphicsLineItem* _tip;
    //! The body of the ruler
    QGraphicsLineItem* _body;
    //! Text to be displayed (picking positions and length)
    QGraphicsTextItem* _basetext;
    QGraphicsTextItem* _tiptext;
    QGraphicsTextItem* _bodytext;
    //! Starting and end point of the ruler
    QPointF _start, _end;
    //! Tip half width
    double _hwidth;
    //! Whether the ruler is calibrated or not
    bool _calibrated;
    //! Pixels to distance conversion factor
    double _conversion;
    bool _displaydistance;
};

#endif // OHKL_GUI_GRAPHICS_TOOLS_RULERITEM_H
