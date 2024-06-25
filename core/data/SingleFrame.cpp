//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/SingleFrame.cpp
//! @brief     Implements class SingleFrame
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/SingleFrame.h"

#include "core/data/DataTypes.h"

#include <stdexcept>

namespace ohkl {

SingleFrame::SingleFrame(const std::string& dataset_name, Diffractometer* diffractometer)
    : DataSet(dataset_name, diffractometer)
{
}

void SingleFrame::addFrame(const std::string& rawfilename, DataFormat format)
{
    if (nFrames() > 0)
        throw std::runtime_error("Can't add more than one frame to a SingleFrame instance");
    else
        DataSet::addFrame(rawfilename, format);
}

Eigen::MatrixXi SingleFrame::frame(const std::size_t idx) const
{
    std::ignore = idx;
    return DataSet::frame(0);
}

Eigen::MatrixXd SingleFrame::transformedFrame(std::size_t idx) const
{
    std::ignore = idx;
    return DataSet::transformedFrame(0);
}

Eigen::MatrixXd SingleFrame::gradientFrame(
    std::size_t idx, GradientKernel kernel, bool realspace) const
{
    std::ignore = idx;
    return DataSet::gradientFrame(0, kernel, realspace);
}

void SingleFrame::setNFrames(std::size_t nframes)
{
    _metadata.add<int>(ohkl::at_frameCount, nframes);
}

} // namespace ohkl
