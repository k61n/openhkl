//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/crystal/TestQShape.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>

#include "base/utils/ProgressHandler.h"
#include "core/algo/DataReaderFactory.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/raw/IDataReader.h"

nsx::Ellipsoid toDetectorSpace(const nsx::Ellipsoid e, const nsx::sptrDataSet data)
{
    auto events = nsx::algo::qs2events(
        {nsx::ReciprocalVector(e.center())}, data->instrumentStates(), data->detector(),
        data->nFrames());

    // something bad happened
    if (events.size() != 1)
        throw std::runtime_error("could not transform ellipse from q space to detector space");

    const auto& event = events[0];
    // auto position =
    //    data->reader()->getDiffractometer()->detector()->pixelPosition(event._px, event._py);
    auto state = data->instrumentStates().interpolate(event._frame);

    // Jacobian of map from detector coords to sample q space
    Eigen::Matrix3d J = state.jacobianQ(event._px, event._py);
    const Eigen::Matrix3d det_inv_cov = J.transpose() * e.metric() * J;

    Eigen::Vector3d p(event._px, event._py, event._frame);
    return nsx::Ellipsoid(p, det_inv_cov);
}


TEST_CASE("test/crystal/TestQShape.cpp", "")
{
    nsx::DataReaderFactory factory;
    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.getDiffractometer()));
    experiment.addData(dataf);

    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);
    nsx::PeakFinder peakFinder;

    auto callback = [progressHandler]() {
        auto log = progressHandler->getLog();
        for (const auto& msg : log)
            std::cout << msg << std::endl;
    };

    progressHandler->setCallback(callback);

    nsx::DataList numors;
    numors.push_back(dataf);

    // propagate changes to peak finder
    auto finder_params = peakFinder.parameters();
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->maximum_frames = 10;
    finder_params->threshold = 15;
    finder_params->peak_end = 1.0;

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular", {});
    peakFinder.setConvolver(std::unique_ptr<nsx::Convolver>(convolver));


    peakFinder.setHandler(progressHandler);

    peakFinder.find(numors);
    auto found_peaks = peakFinder.currentPeaks();

    try {
        CHECK(static_cast<int>(found_peaks.size()) >= 0);
    } catch (...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    CHECK(found_peaks.size() >= 800);

    int good_shapes = 0;

    for (auto peak : found_peaks) {
        if (!peak->enabled())
            continue;

        nsx::Ellipsoid qshape;
        try {
            qshape = peak->qShape();
        } catch (std::range_error& e) {
            continue;
        }
        nsx::Ellipsoid new_shape;
        try {
            new_shape = toDetectorSpace(qshape, dataf);
        } catch (...) {
            continue;
        }
        const auto& old_shape = peak->shape();

        // note: some blobs are invalid, so we skip them
        if (!(old_shape.metric().norm() < 1e3))
            continue;

        ++good_shapes;

        auto dx = new_shape.center() - old_shape.center();

        // transformation x -> q -> x should have sub-pixel accuracy
        CHECK(std::abs(dx.norm()) < 0.01);

        double error = (new_shape.metric() - old_shape.metric()).norm();
        CHECK(std::abs(error) < 2e-2);
    }

    CHECK(good_shapes > 600);
}
