//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/ISigmaIntegrator.cpp
//! @brief     Implements class ISigmaIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/ISigmaIntegrator.h"

#include "base/geometry/Ellipsoid.h"
#include "core/data/DataSet.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/Profile.h"

namespace ohkl {

ISigmaIntegrator::ISigmaIntegrator() : PixelSumIntegrator() { }

ComputeResult ISigmaIntegrator::compute(
    Peak3D* peak, Profile* profile, const IntegrationRegion& region)
{
    ComputeResult result;
    result.integrator_type = IntegratorType::ISigma;

    if (!profile) {
        result.integration_flag = RejectionFlag::NoProfile;
        return result;
    }

    // first get mean background
    const ComputeResult pxsum_result = PixelSumIntegrator::compute(peak, profile, region);
    const double mean_bkg = pxsum_result.sum_background.value();
    const double var_bkg = pxsum_result.sum_background.variance();

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    // TODO: should this be hard-coded??
    if (events.size() < 29) {
        result.integration_flag = RejectionFlag::TooFewPoints;
        return result;
    }

    const Eigen::Vector3d c = peak->shape().center();
    const Eigen::Matrix3d A = peak->shape().metric();

    Profile1D profile1d;
    std::vector<Intensity> mean_profile = profile->profile1d()->profile();

    // evaluate the model profile at the given events
    for (int i = 0; i < events.size(); ++i) {
        Eigen::Vector3d dx(events[i].px, events[i].py, events[i].frame);
        dx -= c;
        const double r2 = dx.transpose() * A * dx;
        profile1d.addPoint(r2, counts[i]);
    }

    int best_idx = -1;
    double best_val = std::numeric_limits<double>::max();

    // now compute minimum of sigma(I)^2 / I^2 + sigma(p)^2 / p^2
    for (int i = 0; i < profile1d.counts().size(); ++i) {
        const double M = profile1d.counts()[i];
        const int n = profile1d.npoints()[i];
        const double I = M - n * mean_bkg;
        const double var_I = M + n * n * var_bkg;

        const double p = mean_profile[i].value();
        const double var_p = mean_profile[i].variance();

        const double val = var_I / I / I + var_p / p / p;

        if (val < best_val) {
            best_val = val;
            best_idx = i;
        }
    }

    // something went wrong (nans?)
    if (best_idx < 0) {
        result.integration_flag = RejectionFlag::NoISigmaMinimum;
        return result;
    }

    const double M = profile1d.counts()[best_idx];
    const int n = profile1d.npoints()[best_idx];

    result.profile_intensity = Intensity(M - n * mean_bkg, M + n * n * var_bkg);
    result.profile_intensity = result.profile_intensity / mean_profile[best_idx];

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
