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

#include <memory>

#include "blosc.h"

#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/parser/BloscFilter.h"
#include "core/parser/EigenToVector.h"
#include "core/loader/FakeDataReader.h"
#include "base/utils/Units.h"

namespace nsx {

FakeDataReader::FakeDataReader(const std::string& filename, Diffractometer* diffractometer)
    : HDF5MetaDataReader(filename, diffractometer)
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
