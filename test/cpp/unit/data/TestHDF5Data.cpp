//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestHDF5Data.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/loader/IDataReader.h"
#include "core/raw/DataKeys.h"
#include "core/instrument/Diffractometer.h"

#include <Eigen/Dense>

TEST_CASE("test/data/TestHDF5Data.cpp", "")
{
    ohkl::Diffractometer* diffractometer = ohkl::Diffractometer::create("BioDiff");
    const ohkl::sptrDataSet dataset_ptr { std::make_shared<ohkl::DataSet>
          ("h5_example", diffractometer) };
    dataset_ptr->addDataFile("H5_example.hdf", ohkl::DataFormat::OHKL);
    dataset_ptr->finishRead();

    Eigen::MatrixXi v = dataset_ptr->frame(0);
    // std::cout << v << std::endl;

    // Check the total number of count in the frame 0
    CHECK(v.sum() == 1282584565);

    dataset_ptr->close();
}
