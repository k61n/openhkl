//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/simpleplot.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_GRAPHICS_SIMPLEPLOT_H
#define GUI_GRAPHICS_SIMPLEPLOT_H

#include <string>
#include "gui/graphics/nsxplot.h"

class QWidget;

class SimplePlot : public NSXPlot {
    Q_OBJECT
public:
    static NSXPlot* create(QWidget* parent);

    explicit SimplePlot(QWidget* parent = 0);

    // Getters and setters
    std::string getType() const;
};

#endif //GUI_GRAPHICS_SIMPLEPLOT_H
