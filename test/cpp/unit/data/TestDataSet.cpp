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
        "CrChiA_c01runab_28603.raw",
        "CrChiA_c01runab_28604.raw",
        "CrChiA_c01runab_28605.raw",
        "CrChiA_c01runab_28606.raw",
        "CrChiA_c01runab_28607.raw",
        "CrChiA_c01runab_28608.raw",
        "CrChiA_c01runab_28609.raw",
        "CrChiA_c01runab_28610.raw",
        "CrChiA_c01runab_28611.raw",
        "CrChiA_c01runab_28612.raw",
        "CrChiA_c01runab_28613.raw",
        "CrChiA_c01runab_28614.raw",
        "CrChiA_c01runab_28615.raw",
        "CrChiA_c01runab_28616.raw",
        "CrChiA_c01runab_28617.raw"};

    const std::string filename = "CrChiA.ohkl";
    ohkl::Experiment expt("test", "BioDiff");
    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>("CrChiA", expt.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::RAW;
    data_params.wavelength = 2.669;
    data_params.delta_omega = 0.3;
    data->setImageReaderParameters(data_params);
    for (const auto& file : filenames)
        data->addFrame(file, DataFormat::RAW);
    data->finishRead();
    expt.addData(data);

    data->initBuffer(true);

    CHECK(data->nFrames() == 15);
    CHECK(data->nRows() == 900);
    CHECK(data->nCols() == 2500);

    ohkl::Detector* detector = data->diffractometer()->detector();
    detector->setBaseline(227.0);
    detector->setGain(7.0);

    CHECK(data->_frame_buffer[5]->sum() == 1254568494);

    ohkl::MetaData metadata = data->metadata();

    Eigen::MatrixXi frame = data->frame(5);
    // raw image
    CHECK(frame.sum() == 1254568494);
    Eigen::MatrixXd transformed_frame = data->transformedFrame(5);
    // baseline/gain-adjusted image
    CHECK(transformed_frame.sum() == Approx(106259784.8571437597).epsilon(eps));

    data->getIntensityHistogram(100);
    gsl_histogram* hist = data->getTotalHistogram();
    CHECK(gsl_histogram_max_bin(hist) == 0);
    CHECK(gsl_histogram_max_val(hist) == Approx(28542331.0).epsilon(eps));
    CHECK(gsl_histogram_mean(hist) == Approx(440.3043524003).epsilon(eps));
    CHECK(gsl_histogram_sigma(hist) == Approx(305.3284398832).epsilon(eps));
    CHECK(gsl_histogram_sum(hist) == Approx(33749999.0).epsilon(eps));
}

TEST_CASE("test/data/TestDataSet.cpp", "")
{
    UnitTest_DataSet::run();
}

}
