//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/Qs2Events.cpp
//! @brief     Implements function qs2events
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/Qs2Events.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h" // TODO mv interpolatedState
#include "core/instrument/InterpolatedState.h"

namespace nsx {

std::vector<DetectorEvent> algo::qs2events(
    const std::vector<ReciprocalVector>& sample_qs, const InstrumentStateList& states,
    const Detector& detector, const int n_intervals, sptrProgressHandler handler /* = nullptr */)
{
    nsxlog(Level::Debug, "algo::Qs2Events::qs2events: processing ", sample_qs.size(), "q vectors");

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

    // Tolerance for bisection search
    const double eps = 1.0e-10;

    const double fmin = 0.0;
    const double fmax = states.size() - 2;

    std::vector<double> roots;
    std::vector<DetectorEvent> events;

    // return true if |k_f| < |k_i|, i.e q_vect is inside Ewald sphere
    auto compute_sign = [](const Eigen::RowVector3d& q, const InterpolatedState& state) -> bool {
        const Eigen::RowVector3d ki = state.ki().rowVector();
        const Eigen::RowVector3d kf = ki + q * state.sampleOrientationMatrix().transpose();
        return kf.squaredNorm() < ki.squaredNorm();
    };

    int count = 0;
    if (handler) {
        std::ostringstream oss;
        oss << "Transforming " << sample_qs.size() << " q-vectors to detector events";
        handler->setStatus(oss.str().c_str());
        handler->setProgress(0);
    }

    // for each sample q, determine the rotation that makes it intersect the Ewald sphere
    for (const ReciprocalVector& sample_q : sample_qs) {
        const Eigen::RowVector3d& q_vect = sample_q.rowVector();

        roots.clear();

        for (int i = 0; i < n_intervals; ++i) { // Iterate over equally sized intervals

            // Generate the interval
            double f0 = fmin + double(i) * fmax/double(n_intervals);
            double f1 = f0 + fmax/double(n_intervals);

            InterpolatedState state0 = states.interpolate(f0);
            InterpolatedState state1 = states.interpolate(f1);

            bool s0 = compute_sign(q_vect, state0);
            bool s1 = compute_sign(q_vect, state1);

            // does not cross Ewald sphere, or crosses more than once
            if (s0 == s1)
                continue;

            // now use bisection method to compute intersection to good accuracy
            while (f1 - f0 > eps) {
                double f = 0.5 * (f0 + f1);
                InterpolatedState state = states.interpolate(f);
                bool sign = compute_sign(q_vect, state);

                if (sign == s0) { // branch right
                    s0 = sign;
                    state0 = state;
                    f0 = f;
                } else { // branch left
                    s1 = sign;
                    state1 = state;
                    f1 = f;
                }
            }
            // store the frame value at the intersection
            roots.push_back(0.5 * (f0 + f1));
        }

        for (const double& frame : roots) { // Generate an event for each frame value
            const auto state = states.interpolate(frame);

            Eigen::RowVector3d kf =
                state.ki().rowVector() + q_vect * state.sampleOrientationMatrix().transpose();
            DetectorEvent event = detector.constructEvent(
                DirectVector(state.samplePosition), ReciprocalVector(kf * state.detectorOrientation),
                frame);
            if (event._tof <= 0)
                continue;

            events.emplace_back(event);
        }
        if (handler)
            handler->setProgress(++count * 100.0 / sample_qs.size());
    }
    nsxlog(Level::Debug, "algo::Qs2Events::qs2events: finished; generated ", events.size(), "events");
    return events;
}

} // namespace nsx
