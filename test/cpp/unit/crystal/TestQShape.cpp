//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/crystal/TestQShape.cpp
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

#include <Eigen/Dense>

#include "base/utils/ProgressHandler.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/raw/DataKeys.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/instrument/InterpolatedState.h"

#include <iostream>


ohkl::Ellipsoid toDetectorSpace(const ohkl::Ellipsoid e, const ohkl::sptrDataSet data)
{
    auto events = ohkl::algo::qVectorList2Events(
        {ohkl::ReciprocalVector(e.center())}, data->instrumentStates(), data->detector(),
        data->nFrames());

    // something bad happened
    if (events.size() != 1)
        throw std::runtime_error("could not transform ellipse from q space to detector space");

    const auto& event = events[0];
    // auto position =
    //    data->reader()->getDiffractometer()->detector()->pixelPosition(event.px, event.py);
    auto state = ohkl::InterpolatedState::interpolate(data->instrumentStates(), event.frame);

    // Jacobian of map from detector coords to sample q space
    Eigen::Matrix3d J = state.jacobianQ(event.px, event.py);
    const Eigen::Matrix3d det_inv_cov = J.transpose() * e.metric() * J;

    Eigen::Vector3d p(event.px, event.py, event.frame);
    return ohkl::Ellipsoid(p, det_inv_cov);
}


TEST_CASE("test/crystal/TestQShape.cpp", "")
{
    ohkl::Experiment experiment("gal3", "BioDiff");
    const ohkl::sptrDataSet dataset_ptr { std::make_shared<ohkl::DataSet>
          ("gal3", experiment.getDiffractometer()) };

    dataset_ptr->addDataFile("gal3.hdf", ohkl::DataFormat::OHKL);
    dataset_ptr->finishRead();
    experiment.addData(dataset_ptr);

    ohkl::sptrProgressHandler progressHandler(new ohkl::ProgressHandler);
    ohkl::PeakFinder peakFinder;

    auto callback = [progressHandler]() {
        auto log = progressHandler->getLog();
        for (const auto& msg : log)
            std::cout << msg << std::endl;
    };

    progressHandler->setCallback(callback);

    // propagate changes to peak finder
    auto finder_params = peakFinder.parameters();
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->maximum_frames = 10;
    finder_params->threshold = 15;
    finder_params->peak_end = 1.0;

    ohkl::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular", {});
    // peakFinder.setConvolver(std::unique_ptr<ohkl::Convolver>(convolver));


    peakFinder.setHandler(progressHandler);

    peakFinder.find(dataset_ptr);
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

        ohkl::Ellipsoid qshape;
        try {
            qshape = peak->qShape();
        } catch (std::range_error& e) {
            continue;
        }
        ohkl::Ellipsoid new_shape;
        try {
            new_shape = toDetectorSpace(qshape, dataset_ptr);
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
