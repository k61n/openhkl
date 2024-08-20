//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestRawDataReader.cpp
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
#include "core/loader/RawDataReader.h"


TEST_CASE("test/data/TestRawDataReader.cpp", "")
{
    ohkl::Diffractometer* diffractometer = ohkl::Diffractometer::create("BioDiff");
    const std::vector<std::string> filenames = {
        "CrChiA_c01runab_28603.raw", "CrChiA_c01runab_28604.raw", "CrChiA_c01runab_28605.raw"};

    const ohkl::sptrDataSet data = std::make_shared<ohkl::DataSet>("CrChiA", diffractometer);

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::RAW;
    data_params.wavelength = 2.669;
    data_params.delta_omega = 0.3;
    data_params.bytes_per_pixel = 2;

    ohkl::RawDataReader reader;
    reader.setDataSet(data.get());
    reader.initRead();
    reader.setParameters(data_params);

    for (const auto& file : filenames)
        reader.addFrame(file);

    Eigen::MatrixXi frame1 = reader.data(0);
    Eigen::MatrixXi frame2 = reader.data(1);
    Eigen::MatrixXi frame3 = reader.data(2);

    CHECK(frame1.sum() == 1238373942);
    CHECK(frame2.sum() == 1237425850);
    CHECK(frame3.sum() == 1234681254);
}
