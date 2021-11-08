//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/RegionData.cpp
//! @brief     Implements class RegionData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/RegionData.h"

namespace nsx {

void RegionData::addFrame(Eigen::MatrixXi& frame, Eigen::MatrixXi& mask)
{
    _data.emplace_back(frame);
    _mask.emplace_back(mask);
}

Eigen::MatrixXi RegionData::frame(size_t i)
{
    if (i < _data.size())
        return _data[i];
    else
        throw std::range_error("Region frame index out of bounds");
}

Eigen::MatrixXi RegionData::mask(size_t i)
{
    if (i < _mask.size())
        return _mask[i];
    else
        throw std::range_error("Mask frame index out of bounds");
}

int RegionData::nFrames() const
{
    return _data.size();
}

} // namespace nsx
