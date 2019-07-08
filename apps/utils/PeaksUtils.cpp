//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/utils/PeaksUtils.cpp
//! @brief     Implements ###THINGS###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <limits>

#include "core/peak/Peak3D.h"

#include "apps/utils/PeaksUtils.h"

std::pair<double, double> dRange(const nsx::PeakList& peaks)
{
    //TODO remove:
    //std::pair<double, double> drange;
    double dmin = std::numeric_limits<double>::infinity();
    double dmax = -std::numeric_limits<double>::infinity();

    for (auto peak : peaks) {
        double d = 1.0 / peak->q().rowVector().norm();
        dmin = std::min(dmin, d);
        dmax = std::max(dmax, d);
    }

    return std::make_pair(dmin, dmax);
}
