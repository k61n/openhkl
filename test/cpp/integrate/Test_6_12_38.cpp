#include "test/cpp/catch.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "core/algo/AutoIndexer.h"
#include "tables/crystal/UnitCell.h"
#include "core/detector/DetectorEvent.h"
#include "core/peak/Peak3D.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "base/geometry/DirectVector.h"
#include "base/geometry/Ellipsoid.h"
#include "base/geometry/ReciprocalVector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/shape/ShapeLibrary.h"
#include "core/algo/DataReaderFactory.h"
#include "core/peak/Peak3D.h"
#include "core/merge_and_filter/PeakFilter.h"
#include "core/search_peaks/PeakFinder.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"

TEST_CASE("test/integrate/Test_6_12_38.cpp", "")
{

    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.diffractometer()));

    experiment.addData(dataf);

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
    nsx::Ellipsoid shape(Eigen::Vector3d(434, 802, 10), 2);
    auto peak = std::make_shared<nsx::Peak3D>(dataf, shape);
    peak->setSelected(true);

    Eigen::Vector3d q1 = peak->q().rowVector();
    CHECK(std::abs((q1 - q0).norm() / q0.norm()) < 1e-1);

    std::cout << q0.transpose() << std::endl;
    std::cout << q1.transpose() << std::endl;

    nsx::PeakList peaks;
    peaks.push_back(peak);
    nsx::PixelSumIntegrator integrator(false, false);
    integrator.integrate(peaks, dataf, 2.7, 3.0, 4.0);

    CHECK(peak->enabled() == true);
}
