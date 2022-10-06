//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/RegionData.cpp
//! @brief     Implements class RegionData
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/RegionData.h"

#include "core/peak/IntegrationRegion.h"

namespace ohkl {

RegionData::RegionData(
    IntegrationRegion* region, double x1, double x2, double y1, double y2, unsigned int z1,
    unsigned int z2)
    : _integration_region(region), _xmin(x1), _xmax(x2), _ymin(y1), _ymax(y2), _zmin(z1), _zmax(z2)
{
}

void RegionData::addFrame(unsigned int frame_index, Eigen::MatrixXi& frame, Eigen::MatrixXi& mask)
{
    _index.emplace_back(frame_index);
    _data.emplace_back(frame);
    _mask.emplace_back(mask);
}

int RegionData::index(std::size_t i)
{
    if (i < _index.size())
        return _index[i];
    else
        throw std::range_error("Frame index out of bounds");
}

Eigen::MatrixXi RegionData::frame(std::size_t i)
{
    if (i < _data.size())
        return _data[i];
    else
        throw std::range_error("Region frame index out of bounds");
}

Eigen::MatrixXi RegionData::mask(std::size_t i)
{
    if (i < _mask.size())
        return _mask[i];
    else
        throw std::range_error("Mask frame index out of bounds");
}

unsigned int RegionData::nFrames() const
{
    return _data.size();
}

unsigned int RegionData::centreFrame() const
{
    return std::round(_integration_region->peak()->shape().center()[2]) - _zmin;
}

unsigned int RegionData::getRegionDataIndex(unsigned int frame_index)
{
    for (int i = 0; i < nFrames(); ++i) {
        if (_index[i] == frame_index)
            return i;
    }
    throw std::range_error("Frame is not in this integration region");
}

IntegrationRegion* RegionData::integrationRegion() const
{
    return _integration_region;
}

double RegionData::xmin() const
{
    return _xmin;
}

double RegionData::xmax() const
{
    return _xmax;
}

double RegionData::ymin() const
{
    return _ymin;
}

double RegionData::ymax() const
{
    return _ymax;
}

unsigned int RegionData::zmin() const
{
    return _zmin;
}

unsigned int RegionData::zmax() const
{
    return _zmax;
}

unsigned int RegionData::cols() const
{
    return _xmax - _xmin;
}

unsigned int RegionData::rows() const
{
    return _ymax - _ymin;
}

} // namespace ohkl
