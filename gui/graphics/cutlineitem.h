//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/cutlineitem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_CUTLINEITEM_H
#define GUI_GRAPHICS_CUTLINEITEM_H

#include "gui/graphics/cutteritem.h"

class QGraphicsSceneWheelEvent;
class QWidget;
class NSXPlot;

class CutLineItem : public CutterItem {
public:
    explicit CutLineItem(nsx::sptrDataSet data);

    ~CutLineItem();

    void plot(NSXPlot* plot);

    void wheelEvent(QGraphicsSceneWheelEvent* event);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    int getNPoints() const;

    void setNPoints(int nPoints);

    // Getters and setters
    std::string getPlotType() const;

private:
    int _nPoints;
};

#endif //GUI_GRAPHICS_CUTLINEITEM_H
