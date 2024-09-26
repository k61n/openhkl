//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/Profile3DIntegrator.cpp
//! @brief     Implements class Profile3DIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/Profile3DIntegrator.h"

#include "core/data/DataSet.h"
#include "core/peak/Peak3D.h"
#include "core/shape/ShapeModel.h"

namespace {

void updateFit(
    ohkl::Intensity& I, ohkl::Intensity& B, const std::vector<double>& profile,
    const std::vector<double>& counts)
{
    Eigen::Matrix2d A;
    A.setZero();
    Eigen::Vector2d b(0, 0);
    const size_t n = std::min(profile.size(), counts.size());

    for (size_t i = 0; i < n; ++i) {
        const double p = profile[i];
        const double M = counts[i];
        const double var = B.value() + I.value() * p;

        A(0, 0) += 1 / var;
        A(0, 1) += p / var;
        A(1, 0) += p / var;
        A(1, 1) += p * p / var;

        b(0) += M / var;
        b(1) += M * p / var;
    }

    const Eigen::Matrix2d AI = A.inverse();
    const Eigen::Vector2d& x = AI * b;

    const double new_B = x(0);
    const double new_I = x(1);

    // check this calculation!
    Eigen::Matrix2d cov = AI;

    // Note: this error estimate assumes the variances are correct (i.e., gain and
    // baseline accounted for)
    B = ohkl::Intensity(new_B, cov(0, 0));
    I = ohkl::Intensity(new_I, cov(1, 1));
}

}

namespace ohkl {

ComputeResult Profile3DIntegrator::compute(
    Peak3D* peak, Profile* profile, const IntegrationRegion& region)
{
    ComputeResult result;
    result.integrator_type = IntegratorType::Profile3D;

    if (!profile) {
        result.integration_flag = RejectionFlag::NoProfile;
        return result;
    }

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    // TODO: should this be hard-coded??
    if (events.size() < 29) {
        result.integration_flag = RejectionFlag::TooFewPoints;
        return result;
    }

    // dummy value for initial guess
    result.profile_background = Intensity(1.0, 1.0);
    result.profile_intensity = Intensity(0.0, 0.0);

    std::vector<double> profile_counts;
    std::vector<double> obs_counts;

    profile_counts.reserve(events.size());
    obs_counts.reserve(events.size());

    const double tolerance = 1e-5;

    const DetectorEvent event(peak->shape().center());
    const Profile3D& model_profile = profile->profile3d();
    const PeakCoordinateSystem coord(peak);

    // evaluate the model profile at the given events
    for (int i = 0; i < events.size(); ++i) {
        Eigen::Vector3d x;
        if (model_profile.detectorCoords()) {
            x(0) = events[i].px;
            x(1) = events[i].py;
            x(2) = events[i].frame;
            x -= peak->shape().center();
        } else {
            x = coord.transform(events[i]);
        }

        const double predict = model_profile.predict(x);

        profile_counts.push_back(predict);
        obs_counts.push_back(counts[i]);
    }

    // todo: stopping criterion
    for (auto i = 0; i < 20; ++i) {
        const Intensity old_intensity = result.profile_intensity;
        const double I0 = result.profile_intensity.value();
        updateFit(result.profile_intensity, result.profile_background, profile_counts, obs_counts);
        const double I1 = result.profile_intensity.value();

        if (std::isnan(I1) || std::isnan(result.profile_background.value())) {
            result.profile_intensity = old_intensity;
            break;
        }

        if (I1 < 0.0 || (I1 < (1 + tolerance) * I0 && I0 < (1 + tolerance) * I1))
            break;
    }

    const double sigma = result.profile_intensity.sigma();

    if (std::isnan(sigma) && sigma > 0) {
        result.integration_flag = RejectionFlag::InvalidSigma;
        return result;
    }
    result.sum_intensity = {};
    result.sum_background = {};

    return result;
}

} // namespace ohkl
