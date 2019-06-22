//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/CutLineGraphicsItem.h
//! @brief     Defines class CutLineGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <string>

#include <QGraphicsItem>

#include "core/experiment/DataTypes.h"

#include "apps/items/CutterGraphicsItem.h"

class QGraphicsSceneWheelEvent;
class QWidget;
class SXPlot;

class CutLineGraphicsItem : public CutterGraphicsItem {
 public:
    explicit CutLineGraphicsItem(nsx::sptrDataSet data);

    ~CutLineGraphicsItem();

    void plot(SXPlot* plot);

    void wheelEvent(QGraphicsSceneWheelEvent* event);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    int getNPoints() const;

    void setNPoints(int nPoints);

    // Getters and setters
    std::string getPlotType() const;

 private:
    int _nPoints;
};
