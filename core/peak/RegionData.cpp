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

#include "core/peak/IntegrationRegion.h"

namespace nsx {

RegionData::RegionData(
    IntegrationRegion* region, double x1, double x2, double y1, double y2,
    unsigned int z1, unsigned int z2)
    : _integration_region(region)
    , _xmin(x1)
    , _xmax(x2)
    , _ymin(y1)
    , _ymax(y2)
    , _zmin(z1)
    , _zmax(z2)
{
}

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

unsigned int RegionData::nFrames() const
{
    return _data.size();
}

unsigned int RegionData::centreFrame() const
{
    return std::round(_integration_region->peak()->shape().center()[2]) - _zmin;
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

} // namespace nsx
