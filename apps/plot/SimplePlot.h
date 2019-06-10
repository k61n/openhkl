//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/plot/SimplePlot.h
//! @brief     Defines class SimplePlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

class SimplePlot : public SXPlot {
    Q_OBJECT
public:
    static SXPlot* create(QWidget* parent);

    explicit SimplePlot(QWidget* parent = 0);

    // Getters and setters
    std::string getType() const;
};
