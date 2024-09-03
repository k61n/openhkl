//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestShapeModel.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/DataSet.h"
#include "core/detector/DetectorEvent.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeModel.h"
#include "core/experiment/ShapeModelBuilder.h"
#include "test/cpp/catch.hpp"
#include "core/experiment/Experiment.h"
#include "core/loader/RawDataReader.h"
#include "core/convolve/AnnularConvolver.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakFilter.h"


#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

TEST_CASE("test/data/TestShapeModel.cpp", "")
{
    const std::string filename = "CrChiA.ohkl";
    ohkl::Experiment experiment("test", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrUnitCell cell = experiment.getSptrUnitCell("accepted");
    ohkl::sptrDataSet data = experiment.getData("testdata");
    data->initBuffer(true);

    auto* found_peaks = experiment.getPeakCollection("found");
    found_peaks->computeSigmas();

    auto* filter = experiment.peakFilter();
    auto* filter_flags = filter->flags();
    auto* filter_params = filter->parameters();
    filter_flags->enabled = true;
    filter_flags->d_range = true;
    filter_flags->strength = true;
    filter_params->d_min = 1.5;
    filter_params->d_max = 50.0;
    filter_params->strength_min = 3.0;
    filter_params->strength_max = 1.0E7;
    filter->filter(found_peaks);

    ohkl::PeakCollection fit_peaks{"fit", ohkl::PeakCollectionType::FOUND, data};
    fit_peaks.populateFromFiltered(found_peaks);

    auto* shape_builder = experiment.shapeModelBuilder();
    auto* shape_params = shape_builder->parameters();

    shape_params->sigma_d = found_peaks->sigmaD();
    shape_params->sigma_m = found_peaks->sigmaM();
    shape_params->neighbour_range_pixels = 500;
    shape_params->neighbour_range_frames = 5;
    shape_params->n_subdiv = 5;

    ohkl::ShapeModel shapes = shape_builder->integrate(fit_peaks.getPeakList(), data);
    std::cout << shapes.numberOfPeaks() << "/" << found_peaks->numberOfPeaks()
              << " peaks used in ShapeModel" << std::endl;
    CHECK(shapes.numberOfPeaks() == 260);

    const ohkl::DetectorEvent event(1100, 450, 8);
    // search for neighbours in radius 500 pixels, 5 frames
    ohkl::Profile3D profile = shapes.meanProfile(event);
    ohkl::AABB aabb = profile.ellipsoid().aabb();
    Eigen::Vector3d axis_lengths = aabb.extents();
    std::cout << "Ellipsoid: " << axis_lengths[0] << " x " << axis_lengths[1]
              << " x " << axis_lengths[2] << std::endl;
    // Check that the predicted ellipsoid has the right shape
    REQUIRE_THAT(axis_lengths[0], Catch::Matchers::WithinAbs(0.45, 0.1));
    REQUIRE_THAT(axis_lengths[1], Catch::Matchers::WithinAbs(0.60, 0.1));
    REQUIRE_THAT(axis_lengths[2], Catch::Matchers::WithinAbs(0.43, 0.1));
}
