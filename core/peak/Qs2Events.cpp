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
#include "core/data/DataSet.h" // TODO mv interpolatedState
#include "core/instrument/InterpolatedState.h"

namespace nsx {

std::vector<DetectorEvent> algo::qs2events(
    const std::vector<ReciprocalVector>& sample_qs, const InstrumentStateList& states,
    const Detector& detector)
{
    std::vector<DetectorEvent> events;

    // return true if inside Ewald sphere, false otherwise
    auto compute_sign = [](const Eigen::RowVector3d& q, const InterpolatedState& state) -> bool {
        const Eigen::RowVector3d ki = state.ki().rowVector();
        const Eigen::RowVector3d kf = ki + q * state.sampleOrientationMatrix().transpose();
        return kf.squaredNorm() < ki.squaredNorm();
    };

    // for each sample q, determine the rotation that makes it intersect the Ewald sphere
    for (const ReciprocalVector& sample_q : sample_qs) {
        const Eigen::RowVector3d& q_vect = sample_q.rowVector();

        double f0 = 0.0;
        double f1 = states.size() - 2;

        InterpolatedState state0 = states.interpolate(f0);
        InterpolatedState state1 = states.interpolate(f1);

        bool s0 = compute_sign(q_vect, state0);
        const bool s1 = compute_sign(q_vect, state1);

        // does not cross Ewald sphere, or crosses more than once
        if (s0 == s1)
            continue;

        // now use bisection method to compute intersection to good accuracy
        while (f1 - f0 > 1e-10) {
            double f = 0.5 * (f0 + f1);
            InterpolatedState state = states.interpolate(f);
            bool sign = compute_sign(q_vect, state);

            if (sign == s0) { // branch right
                s0 = sign;
                state0 = state;
                f0 = f;
            } else { // branch left
                state1 = state;
                f1 = f;
            }
        }
        // now f stores the frame value at the intersection
        const double f = 0.5 * (f0 + f1);
        const auto state = states.interpolate(f);

        Eigen::RowVector3d kf =
            state.ki().rowVector() + q_vect * state.sampleOrientationMatrix().transpose();
        DetectorEvent event = detector.constructEvent(
            DirectVector(state.samplePosition), ReciprocalVector(kf * state.detectorOrientation),
            f);
        if (event._tof <= 0)
            continue;

        events.emplace_back(event);
    }
    return events;
}

} // namespace nsx
