//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/EnhancedAnnularImageFilter.h
//! @brief     Defines class EnhancedAnnularImageFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_ENHANCEDANNULARIMAGEFILTER_H
#define OHKL_CORE_IMAGE_ENHANCEDANNULARIMAGEFILTER_H

#include "core/image/AnnularImageFilter.h"

#include "core/convolve/RadialConvolutionKernel.h"

namespace ohkl {

class EnhancedAnnularImageFilter : public AnnularImageFilter {
 public:
    EnhancedAnnularImageFilter(const FilterParameters& params);
    void filter() override;
};

} // namespace ohkl

#endif // OHKL_CORE_IMAGE_ENHANCEDANNULARIMAGEFILTER_H
