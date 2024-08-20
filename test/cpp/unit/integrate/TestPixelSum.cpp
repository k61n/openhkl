//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestPixelSum.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/PixelSumIntegrator.h"
#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"

#include <iostream>

namespace ohkl {

class UnitTest_PixelSumIntegrator {
 public:
    static void run();
};

void UnitTest_PixelSumIntegrator::run()
{
    const double eps = 1.0e-5;

    const std::string filename = "CrChiA.ohkl";
    Experiment experiment("test", "BioDiff");
    experiment.loadFromFile(filename);

    sptrDataSet data = experiment.getData("testdata");
    PeakCollection* peaks = experiment.getPeakCollection("found");

    Peak3D* ref_peak = peaks->getPeakList()[382];

    std::cout << ref_peak->shape().center() << std::endl;

    IntegrationRegion region(ref_peak, 5.5, 1.3, 2.3, RegionType::FixedEllipsoid);

    for (std::size_t idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame = data->transformedFrame(idx);
        Eigen::MatrixXi mask;
        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));
        region.advanceFrame(current_frame, mask, idx);
    }

    PixelSumIntegrator integrator;
    integrator.compute(ref_peak, nullptr, region);

    CHECK(ref_peak->sumIntensity().value() == Approx(3794.3458457107).epsilon(eps));
    CHECK(ref_peak->sumIntensity().sigma() == Approx(12.0125774006).epsilon(eps));
}


TEST_CASE("test/integrate/TestPixelSum.cpp", "")
{
    UnitTest_PixelSumIntegrator::run();
}
}
