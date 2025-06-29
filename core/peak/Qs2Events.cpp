//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/Qs2Events.cpp
//! @brief     Implements function qVectorList2Events
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/Qs2Events.h"

#include "base/geometry/DirectVector.h"
#include "base/utils/Logger.h"
#include "base/utils/ParallelFor.h"
#include "base/utils/ProgressHandler.h"
#include "core/data/DataSet.h" // TODO mv interpolatedState
#include "core/detector/Detector.h"
#include "core/instrument/InterpolatedState.h"
#include "tables/crystal/MillerIndex.h"

#include <mutex>

namespace ohkl {

// Tolerance for bisection search
const double eps = 1.0e-10;

// return true if |k_f| < |k_i|, i.e q_vect is inside Ewald sphere
auto compute_sign = [](const Eigen::RowVector3d& q, const InterpolatedState& state) -> bool {
    const Eigen::RowVector3d ki = state.ki().rowVector();
    const Eigen::RowVector3d kf = ki + q * state.sampleOrientationMatrix().transpose();
    return kf.squaredNorm() < ki.squaredNorm();
};

std::vector<std::pair<MillerIndex, DetectorEvent>> algo::qMap2Events(
    const std::vector<std::pair<MillerIndex, ReciprocalVector>>& sample_qs,
    const InstrumentStateList& states, const Detector& detector, const int n_intervals,
    sptrProgressHandler handler /* = nullptr */, bool thread_parallel /* = true */)
{
    ohklLog(
        Level::Debug, "algo::Qs2Events::qMap2Events: processing ", sample_qs.size(),
        " q-vectors");

    std::vector<std::pair<MillerIndex, DetectorEvent>> events;

    if (handler) {
        std::ostringstream oss;
        oss << "Transforming " << sample_qs.size() << " q-vectors to detector events";
        handler->setStatus(oss.str().c_str());
        handler->setProgress(0);
    }

    // for each sample q, determine the rotation that makes it intersect the Ewald sphere
    std::mutex mutex;
    std::atomic_int count = 1;
    parallel_for(sample_qs.size(), [&](int start, int end) {
        for (int idx = start; idx < end; ++idx) {
            std::vector<DetectorEvent> new_events =
                qVector2Events(sample_qs.at(idx).second, states, detector, n_intervals);
            {
                std::lock_guard<std::mutex> lock(mutex);
                for (auto event : new_events)
                    events.push_back({sample_qs.at(idx).first, event});
            }
            if (handler)
                handler->setProgress(++count * 100.0 / sample_qs.size());
        }
    }, thread_parallel);

    if (handler)
        handler->setProgress(100);
    ohklLog(
        Level::Debug, "algo::Qs2Events::qMap2Events: finished; generated ", events.size(),
        " events");
    return events;
}

std::vector<DetectorEvent> algo::qVectorList2Events(
    const std::vector<ReciprocalVector>& sample_qs, const InstrumentStateList& states,
    const Detector& detector, const int n_intervals, sptrProgressHandler handler /* = nullptr */)
{
    ohklLog(
        Level::Debug, "algo::Qs2Events::qVectorList2Events: processing ", sample_qs.size(),
        " q-vectors");

    std::vector<DetectorEvent> events;

    int count = 1;
    if (handler) {
        std::ostringstream oss;
        oss << "Transforming " << sample_qs.size() << " q-vectors to detector events";
        handler->setStatus(oss.str().c_str());
        handler->setProgress(0);
    }

// for each sample q, determine the rotation that makes it intersect the Ewald sphere
    for (const ReciprocalVector& sample_q : sample_qs) {
        std::vector<DetectorEvent> new_events =
            qVector2Events(sample_q, states, detector, n_intervals);
        for (auto event : new_events)
            events.emplace_back(event);
        if (handler)
            handler->setProgress(++count * 100.0 / sample_qs.size());
    }
    if (handler)
        handler->setProgress(100);
    ohklLog(
        Level::Debug, "algo::Qs2Events::qVectorList2Events: finished; generated ", events.size(),
        " events");
    return events;
}

std::vector<DetectorEvent> algo::qVector2Events(
    const ReciprocalVector& sample_q, const InstrumentStateList& states, const Detector& detector,
    const int n_intervals)
{

    const double fmin = 0.0;
    const double fmax = states.size() - 2;

    std::vector<double> roots;
    std::vector<DetectorEvent> events;


    const Eigen::RowVector3d& q_vect = sample_q.rowVector();
    roots.clear();

    /*
     * Algorithm description
     *
     * For each q-vector corresponding to a Miller index:
     * 1. Divide the frame range into n_intervals equal intervals
     * 2. for each interval f0 -> f1:
     *    a) compute sign of |k_f| - |k_i| for f0 (s0) and f1 (s1).
     *    b) if s1 == s2, continue
     *    c) else use bisection search to compute frame value f at which |k_f| = |k_i|
     * 3. For each intersection f, generate a DetectorEvent, which will be used as a peak centre
     */

    for (int i = 0; i < n_intervals; ++i) { // Iterate over equally sized intervals

        // Generate the interval
        double f0 = fmin + double(i) * fmax / double(n_intervals);
        double f1 = f0 + fmax / double(n_intervals);

        InterpolatedState state0 = InterpolatedState::interpolate(states, f0);
        InterpolatedState state1 = InterpolatedState::interpolate(states, f1);

        if (!(state0.isValid() && state1.isValid())) // interpolation failure
            continue;

        bool s0 = compute_sign(q_vect, state0);
        bool s1 = compute_sign(q_vect, state1);

        // does not cross Ewald sphere, or crosses more than once
        if (s0 == s1)
            continue;

        // now use bisection method to compute intersection to good accuracy
        while (f1 - f0 > eps) {
            const double f = 0.5 * (f0 + f1);
            const InterpolatedState state = InterpolatedState::interpolate(states, f);
            const bool sign = compute_sign(q_vect, state);

            if (sign == s0) { // branch right
                state0 = state;
                f0 = f;
            } else { // branch left
                state1 = state;
                f1 = f;
            }
        }
        // store the frame value at the intersection
        roots.push_back(0.5 * (f0 + f1));
    }

    for (const double& frame : roots) { // Generate an event for each frame value
        const auto state = InterpolatedState::interpolate(states, frame);
        if (!state.isValid())
            continue;

        Eigen::RowVector3d kf =
            state.ki().rowVector() + q_vect * state.sampleOrientationMatrix().transpose();
        DetectorEvent event = detector.constructEvent(
            DirectVector(state.samplePosition), ReciprocalVector(kf * state.detectorOrientation),
            frame);
        if (event.tof <= 0)
            continue;

        events.emplace_back(event);
    }
    return events;
}

std::vector<DetectorEvent> algo::getDirectBeamEvents(
    const InstrumentStateList& states, const Detector& detector)
{
    ohklLog(Level::Debug, "algo::getDirectBeamEvents");

    std::vector<DetectorEvent> events;
    const int nframes = states.size();
    for (int frame = 0; frame < nframes; ++frame) { // Generate an event for each frame value
        const auto state = InterpolatedState::interpolate(states, frame);
        if (!state.isValid())
            continue;

        Eigen::RowVector3d kf = state.ki().rowVector();
        DetectorEvent event = detector.constructEvent(
            DirectVector(state.samplePosition), ReciprocalVector(kf * state.detectorOrientation),
            frame);
        if (event.tof <= 0)
            continue;

        events.emplace_back(event);
    }
    return events;
}

} // namespace ohkl
