//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_tools/CutLineItem.h
//! @brief     Defines class CutLineItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_TOOLS_CUTLINEITEM_H
#define NSX_GUI_GRAPHICS_TOOLS_CUTLINEITEM_H

#include "gui/graphics_tools/CutterItem.h"

//! Cutter item that cuts the data as a line
class CutLineItem : public CutterItem {
 public:
    explicit CutLineItem(ohkl::sptrDataSet data);

    ~CutLineItem();

    void plot(SXPlot* plot) override;

    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    int getNPoints() const;

    void setNPoints(int nPoints);

    // Getters and setters
    std::string getPlotType() const override;

 private:
    int _nPoints;
};

#endif // NSX_GUI_GRAPHICS_TOOLS_CUTLINEITEM_H
