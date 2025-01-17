//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/unit/integrate/TestProfile3DCompute.cpp
//! @brief     Test compute method for pixel sum integration
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/detector/DetectorEvent.h"
#include "core/integration/IIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/ShapeModelBuilder.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "tables/crystal/UnitCell.h"

#include <iostream>

namespace ohkl {

class UnitTest_Profile3DIntegrator {
 public:
    static void run();
};

void UnitTest_Profile3DIntegrator::run()
{
    const double eps = 1.0e-5;

    const int ref_npeaks = 1268;

    const int index_weak = 1131;
    const int ref_nprofiles_weak = 24;
    const ohkl::MillerIndex hkl_weak = {7, 16, -17};
    const Eigen::Vector3d ref_weak_center = {835.4216508288, 668.4556875384, 2.8655152724};
    const ohkl::Intensity ref_weak_intensity = {-1255.0754647948, 3664.9743762012};
    const int index_strong = 1842;
    const int ref_nprofiles_strong = 24;
    const ohkl::MillerIndex hkl_strong = {2, -12, -5};
    const Eigen::Vector3d ref_strong_center = {1478.0614852674, 521.7100719425, 4.0832852392};
    const ohkl::Intensity ref_strong_intensity = {289825.4165382066, 4103414.0567727922};

    const std::string filename = "Trypsin-small.ohkl";
    Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    sptrDataSet data = experiment.getData("Scan I");
    data->initBuffer(true);
    PeakCollection* found_peaks = experiment.getPeakCollection("found");
    PeakCollection* predicted_peaks = experiment.getPeakCollection("predicted");
    UnitCell* cell = experiment.getUnitCell("indexed");
    std::cout << cell->toString() << std::endl;

    std::cout << "baseline = " << data->diffractometer()->detector()->baseline() << std::endl;
    std::cout << "gain = " << data->diffractometer()->detector()->gain() << std::endl;

    Peak3D* weak_peak = predicted_peaks->getPeak(index_weak);
    Peak3D* strong_peak = predicted_peaks->getPeak(index_strong);
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

    CHECK(strong_peak->hkl().h() == hkl_strong.h());
    CHECK(strong_peak->hkl().k() == hkl_strong.k());
    CHECK(strong_peak->hkl().l() == hkl_strong.l());
    CHECK_THAT(strong_peak_center[0], Catch::Matchers::WithinAbs(ref_strong_center[0], eps));
    CHECK_THAT(strong_peak_center[1], Catch::Matchers::WithinAbs(ref_strong_center[1], eps));
    CHECK_THAT(strong_peak_center[2], Catch::Matchers::WithinAbs(ref_strong_center[2], eps));

    auto* shape_builder = experiment.shapeModelBuilder();
    auto* shape_params = shape_builder->parameters();

    found_peaks->computeSigmas();
    shape_params->sigma_d = found_peaks->sigmaD();
    shape_params->sigma_m = found_peaks->sigmaM();
    shape_params->neighbour_range_pixels = 100;
    shape_params->neighbour_range_frames = 10;
    shape_params->n_subdiv = 5;
    shape_params->region_type = ohkl::RegionType::FixedEllipsoid;
    shape_params->fixed_peak_end = 5.5;
    shape_params->fixed_bkg_begin = 1.3;
    shape_params->fixed_bkg_end = 2.3;
    shape_params->strength_min = 5.0;
    shape_params->d_min = 1.5;

    ohkl::ShapeModel shapes = shape_builder->integrate(found_peaks->getPeakList(), data);
    std::cout << shapes.numberOfPeaks() << "/" << found_peaks->numberOfPeaks()
              << " peaks used in ShapeModel" << std::endl;
    CHECK(shapes.numberOfPeaks() > ref_npeaks - 2);
    CHECK(shapes.numberOfPeaks() < ref_npeaks + 2);

    DetectorEvent weak_peak_event(weak_peak_center);
    Profile* weak_peak_profile = shapes.meanProfile(weak_peak_event);
    CHECK(weak_peak_profile->profile3d().nProfiles() == ref_nprofiles_weak);

    DetectorEvent strong_peak_event(weak_peak_center);
    Profile* strong_peak_profile = shapes.meanProfile(weak_peak_event);
    CHECK(strong_peak_profile->profile3d().nProfiles() == ref_nprofiles_strong);


    IntegrationRegion weak_peak_region(weak_peak, 5.5, 1.3, 2.3, RegionType::FixedEllipsoid);
    IntegrationRegion strong_peak_region(strong_peak, 5.5, 1.3, 2.3, RegionType::FixedEllipsoid);

    for (std::size_t idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame = data->transformedFrame(idx);
        Eigen::MatrixXi mask;
        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));
        weak_peak_region.advanceFrame(current_frame, mask, idx);
        strong_peak_region.advanceFrame(current_frame, mask, idx);
    }

    IntegrationParameters integration_params;
    integration_params.integrator_type = IntegratorType::Profile3D;
    integration_params.region_type = ohkl::RegionType::FixedEllipsoid;
    integration_params.fixed_peak_end = 5.5;
    integration_params.fixed_bkg_begin = 1.3;
    integration_params.fixed_bkg_end = 2.3;
    integration_params.use_max_d = false;
    integration_params.use_max_strength = false;

    Profile3DIntegrator integrator;
    integrator.setParameters(integration_params);
    ComputeResult weak_peak_result =
        integrator.compute(weak_peak, weak_peak_profile, weak_peak_region);
    ComputeResult strong_peak_result =
        integrator.compute(strong_peak, strong_peak_profile, strong_peak_region);

    // std::cout << "Weak peak result: " << std::endl << weak_peak_result.toString();
    // std::cout << "Strong peak result: " << std::endl << strong_peak_result.toString();

    CHECK(weak_peak_result.integration_flag == RejectionFlag::NotRejected);
    CHECK(strong_peak_result.integration_flag == RejectionFlag::NotRejected);

    weak_peak->updateIntegration(
        weak_peak_result, integration_params.fixed_peak_end, integration_params.fixed_bkg_begin,
        integration_params.fixed_bkg_end, integration_params.region_type);
    strong_peak->updateIntegration(
        strong_peak_result, integration_params.fixed_peak_end, integration_params.fixed_bkg_begin,
        integration_params.fixed_bkg_end, integration_params.region_type);

    CHECK_THAT(
        weak_peak->profileIntensity().value(),
        Catch::Matchers::WithinAbs(ref_weak_intensity.value(), eps));
    CHECK_THAT(
        weak_peak->profileIntensity().variance(),
        Catch::Matchers::WithinAbs(ref_weak_intensity.variance(), eps));
    CHECK_THAT(
        strong_peak->profileIntensity().value(),
        Catch::Matchers::WithinAbs(ref_strong_intensity.value(), eps));
    CHECK_THAT(
        strong_peak->profileIntensity().variance(),
        Catch::Matchers::WithinAbs(ref_strong_intensity.variance(), eps));
}


TEST_CASE("test/integrate/TestProfile3DCompute.cpp", "")
{
    UnitTest_Profile3DIntegrator::run();
}
}
