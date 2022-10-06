//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestInterpolatedState.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <array>

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/ProgressHandler.h"
#include "core/data/DataSet.h"
#include "core/raw/DataKeys.h"
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/loader/IDataReader.h"

void run_test(const char* filename, const char* instrument)
{
    ohkl::Experiment experiment("test", instrument);
    auto diffractometer = experiment.getDiffractometer();
    const auto* detector = diffractometer->detector();

    const ohkl::sptrDataSet dataset_ptr { std::make_shared<ohkl::DataSet>
         (ohkl::kw_datasetDefaultName, experiment.getDiffractometer()) };

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

    for (auto coord : coords) {
        const double dt = 1e-3;

        auto state = ohkl::InterpolatedState::interpolate(dataset_ptr->instrumentStates(), coord[2]);
        Eigen::Matrix3d Jq = state.jacobianQ(coord[0], coord[1]);

        auto pos0 = detector->pixelPosition(coord[0], coord[1]);
        Eigen::Vector3d q0 = state.sampleQ(pos0).rowVector();

        auto pos1 = detector->pixelPosition(coord[0] + dt, coord[1]);
        Eigen::Vector3d dq1 = state.sampleQ(pos1).rowVector().transpose() - q0;

        auto pos2 = detector->pixelPosition(coord[0], coord[1] + dt);
        Eigen::Vector3d dq2 = state.sampleQ(pos2).rowVector().transpose() - q0;

        auto state3 = ohkl::InterpolatedState::interpolate(dataset_ptr->instrumentStates(), coord[2] + dt);
        Eigen::Vector3d dq3 = state3.sampleQ(pos0).rowVector().transpose() - q0;

        // Numerical Jacobian
        Eigen::Matrix3d NJ;
        NJ.col(0) = dq1 / dt;
        NJ.col(1) = dq2 / dt;
        NJ.col(2) = dq3 / dt;

        CHECK(std::abs((dq1 - Jq * Eigen::Vector3d(dt, 0, 0)).norm() / dq1.norm()) < 1e-3);
        CHECK(std::abs((dq2 - Jq * Eigen::Vector3d(0, dt, 0)).norm() / dq2.norm()) < 1e-3);
        CHECK(std::abs((dq3 - Jq * Eigen::Vector3d(0, 0, dt)).norm() / dq3.norm()) < 1e-3);

        // test numerical vs. analytic Jacobian
        CHECK(std::abs((NJ - Jq).norm() / Jq.norm()) < 1e-5);
    }
}

TEST_CASE("test/instrument/TestInterpolatedState.cpp", "")
{
    run_test("gal3.hdf", "BioDiff2500");
    run_test("d19_test.hdf", "D19");
}
