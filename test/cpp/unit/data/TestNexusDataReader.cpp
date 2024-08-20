//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestNexusDataReader.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/IDataReader.h"
#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/TiffDataReader.h"
#include <string>


TEST_CASE("test/data/TestNexusDataReader.cpp", "")
{
    const double eps = 1.0e-5;

    ohkl::Diffractometer* diffractometer = ohkl::Diffractometer::create("D19");
    const std::vector<std::string> filenames = {"509192.nxs", "509193.nxs", "509194.nxs"};

    ohkl::DataList numors;

    int count = 1;
    for (const auto& file : filenames) {
        const std::string name = "nexus_test_" + std::to_string(++count);
        const ohkl::sptrDataSet data =
            std::make_shared<ohkl::DataSet>(name, diffractometer);
        data->addDataFile(file, ohkl::DataFormat::NEXUS);
        data->finishRead();
        CHECK(data->wavelength() == Approx(1.4561630487).epsilon(eps));
        CHECK(data->nFrames() == 1130);
        CHECK(data->nCols() == 640);
        CHECK(data->nRows() == 256);
        numors.push_back(data);
    }

    CHECK(numors[0]->frame(123).sum() == 3356);
    CHECK(numors[1]->frame(540).sum() == 3301);
    CHECK(numors[2]->frame(887).sum() == 3535);
}
