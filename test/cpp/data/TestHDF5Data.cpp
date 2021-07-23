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

#include "core/data/DataSet.h"
#include "core/raw/DataKeys.h"
#include "core/instrument/Diffractometer.h"

TEST_CASE("test/data/TestHDF5Data.cpp", "")
{
    nsx::Diffractometer* diffractometer = nsx::Diffractometer::create("BioDiff2500");
    const nsx::sptrDataSet dataset_ptr { std::make_shared<nsx::DataSet>
          (nsx::kw_datasetDefaultName, diffractometer) };
    dataset_ptr->addDataFile("H5_example.hdf", "nsx");
    dataset_ptr->finishRead();

    Eigen::MatrixXi v = dataset_ptr->frame(0);
    // std::cout << v << std::endl;

    // Check the total number of count in the frame 0
    CHECK(v.sum() == 1282584565);

    dataset_ptr->close();
}
