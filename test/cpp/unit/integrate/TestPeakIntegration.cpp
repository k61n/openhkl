//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/Test_6_12_38.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/IDataReader.h"
#include "test/cpp/catch.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "base/geometry/DirectVector.h"
#include "base/geometry/Ellipsoid.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/data/DataSet.h"
#include "core/raw/DataKeys.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/ShapeModel.h"
#include "tables/crystal/UnitCell.h"

TEST_CASE("test/integrate/Test_6_12_38.cpp", "")
{
    ohkl::Experiment experiment("gal3", "BioDiff");

    const ohkl::sptrDataSet dataset_ptr { std::make_shared<ohkl::DataSet>
          ("gal3", experiment.getDiffractometer()) };

    dataset_ptr->addDataFile("gal3.hdf", ohkl::DataFormat::OHKL);
    dataset_ptr->finishRead();

    experiment.addData(dataset_ptr);

    Eigen::Matrix3d A;

    A(0, 0) = 2.6701917977711296;
    A(0, 1) = -14.756687297370734;
    A(0, 2) = -52.793228654324409;

    A(1, 0) = -58.414811499919281;
    A(1, 1) = -0.59943589892690052;
    A(1, 2) = -2.7869684459636987;

    A(2, 0) = 0.19723512073382329;
    A(2, 1) = 64.314912973653236;
    A(2, 2) = -17.967236099898955;

    const Eigen::Matrix3d B = A.inverse().transpose();
    const Eigen::Vector3d q0 = Eigen::RowVector3d(-6, 12, -38) * B;
    ohkl::Ellipsoid shape(Eigen::Vector3d(434, 802, 10), 2);
    auto peak = ohkl::Peak3D(dataset_ptr, shape);

    Eigen::Vector3d q1 = peak.q().rowVector();
    CHECK(std::abs((q1 - q0).norm() / q0.norm()) < 1e-1);

    std::cout << q0.transpose() << std::endl;
    std::cout << q1.transpose() << std::endl;

    std::vector<ohkl::Peak3D*> peaks;
    peaks.push_back(&peak);
    ohkl::PixelSumIntegrator integrator;
    ohkl::IntegrationParameters params{};
    params.fit_center = false;
    params.fit_cov = false;
    params.peak_end = 2.7;
    params.bkg_begin = 3.0;
    params.bkg_end = 4.0;
    integrator.setParameters(params);
    integrator.integrate(peaks, nullptr, dataset_ptr);

    CHECK(peak.enabled() == true);
}
