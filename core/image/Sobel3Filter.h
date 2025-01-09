//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/Sobel3Filter.h
//! @brief     Defines class Sobel3Filter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_SOBEL3FILTER_H
#define OHKL_CORE_IMAGE_SOBEL3FILTER_H

#include "core/image/GradientFilter.h"

#include <opencv2/opencv.hpp>

namespace ohkl {

class Sobel3Filter : public GradientFilter {
 public:
    Sobel3Filter();

    void filter() override;
    void filter(GradientDirection direction) override;

 protected:
};

} // namespace ohkl

#endif // OHKL_CORE_IMAGE_SOBEL3FILTER_H
