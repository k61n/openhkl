//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/unit/integrate/TestPixelSumCompute.cpp
//! @brief     Test compute method for pixel sum integration
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/integration/IIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "tables/crystal/MillerIndex.h"

#include <iostream>

namespace ohkl {

class UnitTest_PixelSumIntegrator {
 public:
    static void run();
};

void UnitTest_PixelSumIntegrator::run()
{
    const double eps = 1.0e-5;

    const int index_weak = 1131;
    const ohkl::MillerIndex hkl_weak = {7, 16, -17};
    const Eigen::Vector3d ref_weak_center = {835.4216508288, 668.4556875384, 2.8655152724};
    const ohkl::Intensity ref_weak_intensity = {210.7884348661, 10.8103122695};
    const int index_strong = 1842;
    const ohkl::MillerIndex hkl_strong = {2, -12, -5};
    const Eigen::Vector3d ref_strong_center = {1478.0614852674, 521.7100719425, 4.0832852392};
    const ohkl::Intensity ref_strong_intensity = {12878.5234484999, 13.4893062539};

    const std::string filename = "Trypsin-small.ohkl";
    Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    sptrDataSet data = experiment.getData("Scan I");
    PeakCollection* peaks = experiment.getPeakCollection("predicted");

    std::cout << "baseline = " << data->diffractometer()->detector()->baseline() << std::endl;
    std::cout << "gain = " << data->diffractometer()->detector()->gain() << std::endl;

    Peak3D* weak_peak = peaks->getPeak(index_weak);
    Peak3D* strong_peak = peaks->getPeak(index_strong);
    auto weak_peak_center = weak_peak->shape().center();
    auto strong_peak_center = strong_peak->shape().center();

    std::cout << weak_peak->shape().center().transpose() << std::endl;
    std::cout << strong_peak->shape().center().transpose() << std::endl;

    CHECK(weak_peak->hkl().h() == hkl_weak.h());
    CHECK(weak_peak->hkl().k() == hkl_weak.k());
    CHECK(weak_peak->hkl().l() == hkl_weak.l());
    CHECK_THAT(weak_peak_center[0], Catch::Matchers::WithinAbs(ref_weak_center[0], eps));
    CHECK_THAT(weak_peak_center[1], Catch::Matchers::WithinAbs(ref_weak_center[1], eps));
    CHECK_THAT(weak_peak_center[2], Catch::Matchers::WithinAbs(ref_weak_center[2], eps));
    // hkl symmetric (-1, -20, 20)

    CHECK(strong_peak->hkl().h() == hkl_strong.h());
    CHECK(strong_peak->hkl().k() == hkl_strong.k());
    CHECK(strong_peak->hkl().l() == hkl_strong.l());
    CHECK_THAT(strong_peak_center[0], Catch::Matchers::WithinAbs(ref_strong_center[0], eps));
    CHECK_THAT(strong_peak_center[1], Catch::Matchers::WithinAbs(ref_strong_center[1], eps));
    CHECK_THAT(strong_peak_center[2], Catch::Matchers::WithinAbs(ref_strong_center[2], eps));
    // hkl symmetric (-1, -12, 5)

    IntegrationRegion weak_region(weak_peak, 5.5, 1.3, 2.3, RegionType::FixedEllipsoid);
    IntegrationRegion strong_region(strong_peak, 5.5, 1.3, 2.3, RegionType::FixedEllipsoid);

    for (std::size_t idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame = data->transformedFrame(idx);
        Eigen::MatrixXi mask;
        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));
        weak_region.updateMask(mask, idx);
        strong_region.updateMask(mask, idx);
        weak_region.advanceFrame(current_frame, mask, idx);
        strong_region.advanceFrame(current_frame, mask, idx);
    }

    PixelSumIntegrator integrator;
    ComputeResult weak = integrator.compute(weak_peak, nullptr, weak_region);
    ComputeResult strong = integrator.compute(strong_peak, nullptr, strong_region);

    CHECK(static_cast<int>(weak.integration_flag) == 0);
    CHECK(static_cast<int>(strong.integration_flag) == 0);

    CHECK_THAT(
        weak_peak->sumIntensity().value(),
        Catch::Matchers::WithinAbs(ref_weak_intensity.value(), eps));
    CHECK_THAT(
        weak_peak->sumIntensity().sigma(),
        Catch::Matchers::WithinAbs(ref_weak_intensity.variance(), eps));
    CHECK_THAT(
        strong_peak->sumIntensity().value(),
        Catch::Matchers::WithinAbs(ref_strong_intensity.value(), eps));
    CHECK_THAT(
        strong_peak->sumIntensity().sigma(),
        Catch::Matchers::WithinAbs(ref_strong_intensity.variance(), eps));

    // Rocking curves
    Intensity weak_sum = {0, 0};
    std::cout << "Total (weak) = " << weak.sum_intensity.value() << " "
              << weak.sum_intensity.sigma() << std::endl;
    std::cout << "nframes = " << weak.rocking_curve.size() << std::endl;
    for (std::size_t idx = 0; idx < weak.rocking_curve.size(); ++idx) {
        weak_sum += weak.rocking_curve.at(idx);
        std::cout << idx << " " << weak.rocking_curve.at(idx).value() << " "
                  << weak.rocking_curve.at(idx).sigma() << std::endl;
    }

    Intensity strong_sum = {0, 0};
    std::cout << "Total (strong) = " << strong.sum_intensity.value() << " "
              << strong.sum_intensity.sigma() << std::endl;
    std::cout << "nframes = " << strong.rocking_curve.size() << std::endl;
    for (std::size_t idx = 0; idx < strong.rocking_curve.size(); ++idx) {
        strong_sum += strong.rocking_curve.at(idx);
        std::cout << idx << " " << strong.rocking_curve.at(idx).value() << " "
                  << strong.rocking_curve.at(idx).sigma() << std::endl;
    }

    CHECK_THAT(weak.sum_intensity.value(), Catch::Matchers::WithinAbs(weak_sum.value(), eps));
    // CHECK_THAT(weak.sum_intensity.sigma(), Catch::Matchers::WithinAbs(weak_sum.sigma(), eps));
    CHECK_THAT(strong.sum_intensity.value(), Catch::Matchers::WithinAbs(strong_sum.value(), eps));
    // CHECK_THAT(strong.sum_intensity.sigma(), Catch::Matchers::WithinAbs(strong_sum.sigma(), eps));
}


TEST_CASE("test/integrate/TestPixelSumCompute.cpp", "")
{
    UnitTest_PixelSumIntegrator::run();
}
}
