//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/geometry/TestPeakCoordinateSystem.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "core/data/DataSet.h"
#include "core/raw/DataKeys.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"

void run_test(const char* filename, const char* instrument)
{
    nsx::Experiment experiment("test", instrument);

    const nsx::sptrDataSet dataset_ptr { std::make_shared<nsx::DataSet>
          (nsx::kw_datasetDefaultName, experiment.getDiffractometer()) };

    dataset_ptr->addDataFile(filename, "nsx");
    dataset_ptr->finishRead();

    experiment.addData(dataset_ptr);

    const int nrows = dataset_ptr->nRows();
    const int ncols = dataset_ptr->nCols();

    const int nframes = dataset_ptr->nFrames();

    const std::array<double, 9> fractions = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    // different places to check the coordinate calculation
    std::vector<Eigen::Vector3d> coords;

    for (auto dx : fractions) {
        for (auto dy : fractions) {
            for (auto df : fractions)
                coords.emplace_back(Eigen::Vector3d(dx * ncols, dy * nrows, df * nframes));
        }
    }

    nsx::Peak3D peak(dataset_ptr);

    for (auto coord : coords) {
        peak.setShape(nsx::Ellipsoid(coord, 2.0));
        nsx::PeakCoordinateSystem frame(&peak);

        auto J = frame.jacobian();

        const double dt = 1e-1;

        Eigen::Vector3d e0 = frame.transform(nsx::DetectorEvent(coord[0], coord[1], coord[2]));
        Eigen::Vector3d e1 = frame.transform(nsx::DetectorEvent(coord[0] + dt, coord[1], coord[2]));
        Eigen::Vector3d e2 = frame.transform(nsx::DetectorEvent(coord[0], coord[1] + dt, coord[2]));
        Eigen::Vector3d e3 = frame.transform(nsx::DetectorEvent(coord[0], coord[1], coord[2] + dt));

        Eigen::Vector3d y1 = J * Eigen::Vector3d(dt, 0, 0);
        Eigen::Vector3d y2 = J * Eigen::Vector3d(0, dt, 0);
        Eigen::Vector3d y3 = J * Eigen::Vector3d(0, 0, dt);

        CHECK(std::abs(e0.norm()) < 1e-8);
        std::cout << e0.transpose() << std::endl;
        CHECK(std::abs((e1 - y1).norm() / e1.norm()) < 1e-1);
        CHECK(std::abs((e2 - y2).norm() / e2.norm()) < 1e-1);
        CHECK(std::abs((e3 - y3).norm() / e3.norm()) < 1e-1);

        auto sigmaD = frame.estimateDivergence();
        auto sigmaM = frame.estimateMosaicity();

        auto detector_shape = frame.detectorShape(sigmaD, sigmaM);

        peak.setShape(detector_shape);

        CHECK(frame.estimateDivergence() == Approx(sigmaD).epsilon(1e-6));
        CHECK(frame.estimateMosaicity() == Approx(sigmaM).epsilon(1e-6));

        auto standard_shape = frame.standardShape();
        Eigen::Matrix3d cov = standard_shape.inverseMetric();

        CHECK(cov(0, 0) == Approx(sigmaD * sigmaD).epsilon(1e-4));
        CHECK(cov(1, 1) == Approx(sigmaD * sigmaD).epsilon(1e-4));
        CHECK(cov(2, 2) == Approx(sigmaM * sigmaM).epsilon(1e-4));

        CHECK(std::abs(cov(0, 1)) < 1e-8);
        CHECK(std::abs(cov(0, 2)) < 1e-8);
        CHECK(std::abs(cov(1, 2)) < 1e-8);
    }
}

TEST_CASE("test/geometry/TestPeakCoordinateSystem.cpp", "")
{
    run_test("gal3.hdf", "BioDiff2500");
    run_test("d19_test.hdf", "D19");
}
