//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/data/TestHDF5Data.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <Eigen/Dense>

#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"

TEST_CASE("test/data/TestHDF5Data.cpp", "")
{
    nsx::DataReaderFactory factory;

    nsx::Diffractometer* diffractometer = nsx::Diffractometer::create("BioDiff2500");
    auto dataf = factory.create("hdf", "H5_example.hdf", diffractometer);

    dataf->open();
    Eigen::MatrixXi v = dataf->frame(0);
    // std::cout << v << std::endl;

    // Check the total number of count in the frame 0
    CHECK(v.sum() == 1282584565);

    dataf->close();
}
