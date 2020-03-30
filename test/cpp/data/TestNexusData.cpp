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


TEST_CASE("test/data/TestNexusData.cpp", "")
{
    nsx::DataReaderFactory factory;

    nsx::Diffractometer* diffractometer = nsx::Diffractometer::create("D19");
    std::shared_ptr<nsx::DataSet> datafile;
    try
    {
        datafile = factory.create("nxs", "/home/tw/tmp/nsx/internal-192/501168.nxs", diffractometer);
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Skipping test because data file is not available." << std::endl;
        return;
    }

    datafile->open();
    Eigen::MatrixXi v = datafile->frame(0);
    CHECK((v.rows()==256 && v.cols()==640));
    //std::cout << v << std::endl;

    //CHECK(v.sum() == xxxxxxx);
    datafile->close();
}
