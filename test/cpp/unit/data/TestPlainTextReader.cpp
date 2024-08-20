//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestPlainTextReader.cpp
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
#include "core/instrument/Diffractometer.h"
#include "core/loader/PlainTextReader.h"


TEST_CASE("test/data/TestPlainTextReader.cpp", "")
{
    ohkl::Diffractometer* diffractometer = ohkl::Diffractometer::create("POLI");
    const std::vector<std::string> filenames = {
        "det_pos1_001.dat", "det_pos1_002.dat", "det_pos1_003.dat"};

    const ohkl::sptrDataSet data = std::make_shared<ohkl::DataSet>("poli_test", diffractometer);

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::PLAINTEXT;
    data_params.wavelength = 1.15;
    data_params.delta_omega = 0.1;
    data_params.twotheta_gamma = 38.36;

    ohkl::PlainTextReader reader;
    reader.setDataSet(data.get());
    reader.initRead();
    reader.setParameters(data_params);

    for (const auto& file : filenames)
        reader.addFrame(file);

    Eigen::MatrixXi frame1 = reader.data(0);
    Eigen::MatrixXi frame2 = reader.data(1);
    Eigen::MatrixXi frame3 = reader.data(2);

    CHECK(frame1.sum() == 1056238);
    CHECK(frame2.sum() == 1048964);
    CHECK(frame3.sum() == 1040657);
}
