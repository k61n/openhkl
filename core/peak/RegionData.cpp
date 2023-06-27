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

#include "core/data/DataSet.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/ShapeModel.h"

#include <algorithm>

namespace ohkl {

RegionData::RegionData(
    IntegrationRegion* region, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2,
    unsigned int z1, unsigned int z2)
    : _integration_region(region)
    , _n_profiles(0)
    , _xmin(x1)
    , _xmax(x2)
    , _ymin(y1)
    , _ymax(y2)
    , _zmin(z1)
    , _zmax(z2)
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
    auto it = std::find(_index.begin(), _index.end(), i);
    if (it != _index.end())
        return it - _index.begin();
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

Eigen::MatrixXd RegionData::profileData(std::size_t i)
{
    if (i < _profile_data.size()) {
        return _profile_data[i];
    } else
        throw std::range_error("Region frame index out of bounds");
}

Profile3D* RegionData::profile()
{
    return &_profile;
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

void RegionData::buildProfile(ShapeModel* shapes, double radius, double nframes)
{
    Peak3D* peak = _integration_region->peak();
    DetectorEvent center = {
        peak->shape().center()[0], peak->shape().center()[1], peak->shape().center()[2]};
    auto profile = shapes->meanProfile(center, radius, nframes);
    if (profile)
        _n_profiles = profile.value().nProfiles();

    const auto& events = _integration_region->peakData().events();
    std::vector<double> profile_counts;

    for (const auto& mat : _data)
        _profile_data.emplace_back(Eigen::MatrixXd::Zero(mat.rows(), mat.cols()));

    Eigen::MatrixXi mask;
    mask.resize(peak->dataSet()->nRows(), peak->dataSet()->nCols());
    for (std::size_t frame_idx = 0; frame_idx < peak->dataSet()->nFrames(); ++frame_idx) {
        Eigen::MatrixXd frame = peak->dataSet()->transformedFrame(frame_idx);
        _integration_region->advanceFrame(frame, mask, frame_idx);
    }

    PeakCoordinateSystem system(peak);
    for (int idx = 0; idx < events.size(); ++idx) {
        Eigen::Vector3d coords;
        if (shapes->detectorCoords()) {
            coords(0) = events[idx].px;
            coords(1) = events[idx].py;
            coords(2) = events[idx].frame;
            coords -= peak->shape().center();
        } else {
            coords = system.transform(events[idx]);
        }
        if (!profile)
            throw std::runtime_error("RegionData::buildProfile: bad profile");

        profile_counts.push_back(profile.value().predict(coords));
    }

    for (std::size_t idx = 0; idx < events.size(); ++idx) {
        auto event = events[idx];
        unsigned int frame_ind = index(event.frame);
        unsigned int x_ind = std::round(event.px) - _xmin;
        unsigned int y_ind = std::round(event.py) - _ymin;
        _profile_data[frame_ind](x_ind, y_ind) = profile_counts[idx];
    }
    scaleProfile();
}

void RegionData::scaleProfile()
{
    if (!_integration_region->peak()->sumIntensity().isValid())
        return;

    double intensity = _integration_region->peak()->sumIntensity().value();
    int nrows = _profile_data[0].rows();
    int ncols = _profile_data[0].cols();
    int npixels = nrows * ncols * nFrames();
    double scale = intensity / static_cast<double>(npixels);
    for (auto frame : _profile_data)
        frame *= scale;
}

double RegionData::dataMax() const
{
    int maxval = 0;
    for (auto frame : _data)
        maxval = std::max(maxval, frame.maxCoeff());
    return maxval;
}

double RegionData::profileMax() const
{
    double maxval = 0;
    for (auto frame : _profile_data)
        maxval = std::max(maxval, frame.maxCoeff());
    return maxval;
}

IntegrationRegion* RegionData::integrationRegion() const
{
    return _integration_region;
}

unsigned int RegionData::xmin() const
{
    return _xmin;
}

unsigned int RegionData::xmax() const
{
    return _xmax;
}

unsigned int RegionData::ymin() const
{
    return _ymin;
}

unsigned int RegionData::ymax() const
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
