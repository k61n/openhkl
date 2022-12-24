//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/DataSetGraphics.cpp
//! @brief     Implements class DataSetGraphics
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/DataSetGraphics.h"

DataSetGraphics::DataSetGraphics(DetectorSceneParams* params)
    : _data(nullptr)
    , _color_map(new ColorMap())
    , _params(params)
{
}

std::optional<QImage> DataSetGraphics::baseImage(std::size_t frame_idx, QRect full)
{
    if (!_data)
        return {};

    _current_index = frame_idx;
    _current_frame = _data->frame(frame_idx);
    if (!_params->gradient) {
        return _color_map->matToImage(
            _current_frame.cast<double>(), full, _params->intensity, _params->logarithmic);
    } else {
        return _color_map->matToImage(
            _data->gradientFrame(
                frame_idx, _params->gradient_kernel, !_params->fft_gradient).cast<double>(),
            full, _params->intensity, _params->logarithmic);
    }
}
