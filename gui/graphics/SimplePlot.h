//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/SimplePlot.h
//! @brief     Defines class SimplePlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_SIMPLEPLOT_H
#define NSX_GUI_GRAPHICS_SIMPLEPLOT_H

#include "gui/graphics/SXPlot.h"

//! Specific plot type of the SXPlot
class SimplePlot : public SXPlot {
    Q_OBJECT
 public:
    static SXPlot* create(QWidget* parent);

    explicit SimplePlot(QWidget* parent = 0);

    // Getters and setters
    std::string getType() const;
};

#endif // NSX_GUI_GRAPHICS_SIMPLEPLOT_H
