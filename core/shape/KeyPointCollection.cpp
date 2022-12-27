//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/KeyPointCollection.cpp
//! @brief     Implements class KeyPointCollection
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/KeyPointCollection.h"

#include "core/data/DataSet.h"

namespace ohkl {

KeyPointCollection::KeyPointCollection() : _data(nullptr)
{
}

void KeyPointCollection::setData(sptrDataSet data)
{
    _data = data;
    _keypoint_frames.clear();
    for (std::size_t i = 0; i < data->nFrames(); ++i)
        _keypoint_frames.push_back(std::make_unique<std::vector<cv::KeyPoint>>());
}

std::vector<cv::KeyPoint>* KeyPointCollection::frame(std::size_t idx)
{
    if (!_data)
        return nullptr;
    if (idx >= _data->nFrames())
        return nullptr;
    return _keypoint_frames.at(idx).get();
}

void KeyPointCollection::clearFrame(std::size_t idx)
{
    if (!_data)
        return;
    if (idx >= _data->nFrames())
        return;
    _keypoint_frames.at(idx)->clear();
}

bool KeyPointCollection::hasPeaks(std::size_t idx)
{
    if (!_data)
        return false;
    if (idx >= _data->nFrames())
        return false;
    return !_keypoint_frames.at(idx)->empty();
}

} // namespace ohkl
