//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/CentralDifferenceFilter.h
//! @brief     Defines class CentralDifferenceFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_CENTRALDIFFERENCEFILTER_H
#define OHKL_CORE_IMAGE_CENTRALDIFFERENCEFILTER_H

#include "core/image/GradientFilter.h"

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

namespace ohkl {

class CentralDifferenceFilter : public GradientFilter {
 public:
    CentralDifferenceFilter();

 protected:
};

} // namespace ohkl

#endif // OHKL_CORE_IMAGE_CENTRALDIFFERENCEFILTER_H
