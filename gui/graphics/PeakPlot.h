//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/PeakPlot.h
//! @brief     Defines class PeakPlot
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_PEAKPLOT_H
#define OHKL_GUI_GRAPHICS_PEAKPLOT_H

#include "gui/graphics/SXPlot.h"

//! Specific plot type of SXPlot
class PeakPlot : public SXPlot {
    Q_OBJECT
 public:
    static SXPlot* create(QWidget* parent);

    explicit PeakPlot(QWidget* parent = 0);

    std::string getType() const;
    QCPErrorBars* xErrorBars_; // new QCP 2.0 object
    QCPErrorBars* yErrorBars_;
};


#endif // OHKL_GUI_GRAPHICS_PEAKPLOT_H
