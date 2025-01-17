//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestDataSet.cpp
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
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/raw/MetaData.h"

#include <gsl/gsl_histogram.h>

#include <Eigen/Dense>

namespace ohkl {

const double eps = 1.0e-5;

class UnitTest_DataSet {
 public:
    static void run();
};

void UnitTest_DataSet::run()
{
    const std::vector<std::string> filenames = {
        "p11202_00009983.tiff", "p11202_00009984.tiff", "p11202_00009985.tiff",
        "p11202_00009986.tiff", "p11202_00009987.tiff", "p11202_00009988.tiff",
        "p11202_00009989.tiff", "p11202_00009990.tiff", "p11202_00009991.tiff",
        "p11202_00009992.tiff", "p11202_00009993.tiff", "p11202_00009994.tiff",
        "p11202_00009995.tiff", "p11202_00009996.tiff", "p11202_00009997.tiff",
        "p11202_00009998.tiff", "p11202_00009999.tiff", "p11202_00010000.tiff",
        "p11202_00010001.tiff", "p11202_00010002.tiff"};

    const std::string filename = "Trypsin.ohkl";
    ohkl::Experiment expt("Trypsin", "BioDiff");

    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>("Trypsin", expt.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::TIFF;
    data_params.wavelength = 2.67;
    data_params.delta_omega = 0.4;
    data_params.rebin_size = 1;
    data->setImageReaderParameters(data_params);
    for (const auto& file : filenames)
        data->addFrame(file, DataFormat::TIFF);
    data->finishRead();
    expt.addData(data);

    data->initBuffer(true);

    CHECK(data->nFrames() == 20);
    CHECK(data->nRows() == 1800);
    CHECK(data->nCols() == 5000);

    ohkl::Detector* detector = data->diffractometer()->detector();
    detector->setBaseline(227.0);
    detector->setGain(7.0);

    CHECK(data->_frame_buffer[5]->sum() == 838421048);

    ohkl::MetaData metadata = data->metadata();

    Eigen::MatrixXi frame = data->frame(5);
    // raw image
    CHECK(frame.sum() == 838421048);
    Eigen::MatrixXd transformed_frame = data->transformedFrame(5);
    // baseline/gain-adjusted image
    CHECK(transformed_frame.sum() == Approx(441484049.1428679228).epsilon(eps));

    data->getIntensityHistogram(100);
    gsl_histogram* hist = data->getTotalHistogram();
    CHECK(gsl_histogram_max_bin(hist) == 0);
    CHECK(gsl_histogram_max_val(hist) == Approx(137008911.0).epsilon(eps));
    CHECK(gsl_histogram_mean(hist) == Approx(490.1694910659).epsilon(eps));
    CHECK(gsl_histogram_sigma(hist) == Approx(313.8510008788).epsilon(eps));
    CHECK(gsl_histogram_sum(hist) == Approx(179999976.0).epsilon(eps));
}

TEST_CASE("test/data/TestDataSet.cpp", "")
{
    UnitTest_DataSet::run();
}

}
