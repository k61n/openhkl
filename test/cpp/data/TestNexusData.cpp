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
#include <fstream>
#include <iostream>
#include <vector>

#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"


TEST_CASE("test/data/TestNexusData.cpp", "")
{
    nsx::DataReaderFactory factory;

    std::vector<std::string> files = {
        "/home/tw/tmp/nsx/internal-192/501168.nxs", "/home/tw/tmp/nsx/internal-192/501169.nxs",
        "/home/tw/tmp/nsx/internal-192/501170.nxs", "/home/tw/tmp/nsx/internal-192/501171.nxs",
        "/home/tw/tmp/nsx/internal-192/501172.nxs", "/home/tw/tmp/nsx/internal-192/501173.nxs",
        "/home/tw/tmp/nsx/internal-192/501174.nxs", "/home/tw/tmp/nsx/internal-192/501175.nxs",
        "/home/tw/tmp/nsx/internal-192/501176.nxs", "/home/tw/tmp/nsx/internal-192/501177.nxs",
        "/home/tw/tmp/nsx/internal-192/501178.nxs", "/home/tw/tmp/nsx/internal-192/501179.nxs",
        "/home/tw/tmp/nsx/internal-192/501180.nxs", "/home/tw/tmp/nsx/internal-192/501181.nxs",
        "/home/tw/tmp/nsx/internal-192/501182.nxs", "/home/tw/tmp/nsx/internal-192/501183.nxs",
        "/home/tw/tmp/nsx/internal-192/501184.nxs", "/home/tw/tmp/nsx/internal-192/501185.nxs",
    };

    nsx::Diffractometer* diffractometer = nsx::Diffractometer::create("D19");
    std::vector<std::shared_ptr<nsx::DataSet>> datafiles;

    for (const std::string& file : files) {
        std::cout << "Loading " << file << "..." << std::endl;

        std::ifstream ifstr(file);
        if (!ifstr.is_open()) {
            std::cerr << "Skipping test because the data files are not available" << std::endl;
            return;
        }

        std::shared_ptr<nsx::DataSet> datafile = factory.create("nxs", file, diffractometer);
        datafile->open();
        datafiles.push_back(datafile);
    }

    Eigen::MatrixXi v = datafiles[0]->frame(0);
    CHECK((v.rows() == 256 && v.cols() == 640));
    // std::cout << v << std::endl;

    // CHECK(v.sum() == xxxxxxx);

    for (std::shared_ptr<nsx::DataSet> datafile : datafiles)
        datafile->close();
}
