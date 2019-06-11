//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/PeakPlot.h
//! @brief     Defines class PeakPlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_PEAKPLOT_H
#define GUI_GRAPHICS_PEAKPLOT_H

#include "gui/graphics/SXPlot.h"

class PeakPlot : public NSXPlot {
    Q_OBJECT
public:
    static NSXPlot* create(QWidget* parent);

    explicit PeakPlot(QWidget* parent = 0);

    std::string getType() const;
};


#endif // GUI_GRAPHICS_PEAKPLOT_H
