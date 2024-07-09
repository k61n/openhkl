//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestTiffDataReader.cpp
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
#include "core/loader/TiffDataReader.h"


TEST_CASE("test/data/TestTiffDataReader.cpp", "")
{
    ohkl::Diffractometer* diffractometer = ohkl::Diffractometer::create("BioDiff");
    const std::vector<std::string> filenames = {
        "p11202_00009983.tiff", "p11202_00009984.tiff", "p11202_00009985.tiff"};

    const ohkl::sptrDataSet data1 = std::make_shared<ohkl::DataSet>("Trypsin", diffractometer);
    const ohkl::sptrDataSet data2 = std::make_shared<ohkl::DataSet>("Trypsin", diffractometer);

    ohkl::DataReaderParameters data_params_1;
    data_params_1.data_format = ohkl::DataFormat::TIFF;
    data_params_1.wavelength = 2.67;
    data_params_1.delta_omega = 0.4;
    data_params_1.rebin_size = 2;

    ohkl::TiffDataReader reader1;
    reader1.setDataSet(data1.get());
    reader1.initRead();
    reader1.setParameters(data_params_1);

    for (const auto& file : filenames)
        reader1.addFrame(file);

    Eigen::MatrixXi frame1 = reader1.data(0);
    Eigen::MatrixXi frame2 = reader1.data(1);
    Eigen::MatrixXi frame3 = reader1.data(2);

    CHECK(frame1.sum() == 1283428211);
    CHECK(frame2.sum() == 1283515456);
    CHECK(frame3.sum() == 1282894670);

    ohkl::DataReaderParameters data_params_2;
    data_params_1.data_format = ohkl::DataFormat::TIFF;
    data_params_1.wavelength = 2.67;
    data_params_1.delta_omega = 0.4;
    data_params_1.rebin_size = 1;

    ohkl::TiffDataReader reader2;
    reader2.setDataSet(data2.get());
    reader2.initRead();
    reader2.setParameters(data_params_1);

    for (const auto& file : filenames)
        reader2.addFrame(file);

    frame1 = reader2.data(0);
    frame2 = reader2.data(1);
    frame3 = reader2.data(2);


    CHECK(frame1.sum() == 838746650);
    CHECK(frame2.sum() == 839097405);
    CHECK(frame3.sum() == 836610423);
}
