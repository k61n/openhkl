//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/FakeDataReader.cpp
//! @brief     Implements class FakeDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/FakeDataReader.h"

#include "core/instrument/Diffractometer.h"

#include <memory>

namespace nsx {

FakeDataReader::FakeDataReader(const std::string& filename, Diffractometer* diffractometer)
    : HDF5MetaDataReader<LegacyReader>(filename, diffractometer)
{
    _frames.resize(_nFrames);

    for (size_t i = 0; i < _nFrames; ++i)
        _frames[i].resize(_nRows, _nCols);
}

Eigen::MatrixXi FakeDataReader::data(size_t frame)
{
    return _frames[frame];
}

void FakeDataReader::setData(size_t frame, const Eigen::MatrixXi& data)
{
    _frames[frame] = data;
}

} // namespace nsx
