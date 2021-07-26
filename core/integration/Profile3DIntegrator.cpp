//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/Profile3DIntegrator.cpp
//! @brief     Implements class Profile3DIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/Profile3DIntegrator.h"
#include "core/data/DataSet.h"

namespace nsx {

static void updateFit(
    Intensity& I, Intensity& B, const std::vector<double>& profile,
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

    Eigen::Matrix2d AI = A.inverse();
    const Eigen::Vector2d& x = AI * b;

    const double new_B = x(0);
    const double new_I = x(1);

    // check this calculation!
    Eigen::Matrix2d cov = AI;

    // Note: this error estimate assumes the variances are correct (i.e., gain and
    // baseline accounted for)
    B = Intensity(new_B, cov(0, 0));
    I = Intensity(new_I, cov(1, 1));
}

bool Profile3DIntegrator::compute(
    Peak3D* peak, ShapeCollection* shape_collection, const IntegrationRegion& region)
{
    if (!shape_collection) {
        peak->setRejectionFlag(RejectionFlag::NoShapeCollection);
        return false;
    }

    if (!peak)
        return false;

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    // TODO: should this be hard-coded??
    if (events.size() < 29) {
        peak->setRejectionFlag(RejectionFlag::TooFewPoints);
        throw std::runtime_error("Profile3DIntegrator::compute(): too few data points in peak");
    }

    // dummy value for initial guess
    _meanBackground = Intensity(1.0, 1.0);
    _integratedIntensity = Intensity(0.0, 0.0);

    std::vector<double> profile;
    std::vector<double> obs_counts;

    profile.reserve(events.size());
    obs_counts.reserve(events.size());

    const double tolerance = 1e-5;

    Profile3D model_profile;
    DetectorEvent event(peak->shape().center());

    try {
        // throws if there are no neighboring peaks within the bounds
        model_profile = shape_collection->meanProfile(event, radius(), nFrames());
    } catch (...) {
        peak->setRejectionFlag(RejectionFlag::TooFewNeighbours);
        return false;
    }

    PeakCoordinateSystem coord(peak);

    // evaluate the model profile at the given events
    for (int i = 0; i < events.size(); ++i) {
        Eigen::Vector3d x;
        if (shape_collection->detectorCoords()) {
            x(0) = events[i].px;
            x(1) = events[i].py;
            x(2) = events[i].frame;
            x -= peak->shape().center();
        } else {
            x = coord.transform(events[i]);
        }

        const double predict = model_profile.predict(x);

        // if (predict > 0.0001) {
        profile.push_back(predict);
        obs_counts.push_back(counts[i]);
        //}
    }

    // todo: stopping criterion
    for (auto i = 0; i < 20; ++i) {
        Intensity old_intensity = _integratedIntensity;
        const double I0 = _integratedIntensity.value();
        updateFit(_integratedIntensity, _meanBackground, profile, obs_counts);
        const double I1 = _integratedIntensity.value();

        if (std::isnan(I1) || std::isnan(_meanBackground.value())) {
            _integratedIntensity = old_intensity;
            break;
        }

        if (I1 < 0.0 || (I1 < (1 + tolerance) * I0 && I0 < (1 + tolerance) * I1))
            break;
    }

    double sigma = _integratedIntensity.sigma();

    if (std::isnan(sigma) && sigma > 0) {
        peak->setRejectionFlag(RejectionFlag::InvalidSigma);
        return false;
    }

    return true;
}

} // namespace nsx
