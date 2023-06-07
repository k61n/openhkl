//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/PixelSumIntegrator.cpp
//! @brief     Implements class PixelSumIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/PixelSumIntegrator.h"
#include "base/geometry/Ellipsoid.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/integration/Blob3D.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"

namespace ohkl {

namespace {

std::pair<Intensity, Intensity> compute_background(
    const IntegrationRegion& region, bool use_gradient = false)
{
    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();
    const auto& gradients = region.peakData().gradients();

    if (events.size() < 20) {
        ohklLog(Level::Debug, "compute_background: too few data points");
        return {false, {}};
    }

    // Compute mean and variance. Repeat until no more outliers are to be rejected.
    double mean_bkg = 0;
    double var_bkg;
    double sigma_bkg;
    double mean_grad = 0;
    double var_grad = 0;
    double sigma_grad = 0;
    size_t nbkg;
    for (auto iteration = 0; iteration < 20; ++iteration) {
        double sum_bkg = 0;
        double sum_bkg2 = 0;
        double sum_grad_bkg = 0;
        double sum_grad_bkg2 = 0;
        nbkg = 0;
        for (auto i = 0; i < counts.size(); ++i) {
            if (region.classify(events[i]) != IntegrationRegion::EventType::BACKGROUND)
                continue;
            if (iteration > 0 && std::fabs(counts[i] - mean_bkg) > 3 * sigma_bkg)
                continue;
            sum_bkg += counts[i];
            sum_bkg2 += counts[i] * counts[i];
            if (use_gradient) {
                sum_grad_bkg += gradients[i];
                sum_grad_bkg2 += gradients[i] * gradients[i];
            }
            nbkg++;
        }
        double old_mean = mean_bkg;
        mean_bkg = sum_bkg / nbkg;
        var_bkg = (sum_bkg2 - nbkg * mean_bkg * mean_bkg) / (nbkg - 1);
        sigma_bkg = std::sqrt(var_bkg);
        if (use_gradient) {
            mean_grad = sum_grad_bkg / nbkg;
            var_grad = (sum_grad_bkg2 - nbkg * mean_grad) / (nbkg - 1);
            sigma_grad = std::sqrt(var_grad);
        }

        if (iteration > 0 && std::fabs((old_mean - mean_bkg) / mean_bkg) < 1e-9)
            break;
    }

    if (!use_gradient)
        return {Intensity(mean_bkg, mean_bkg / nbkg), {}};
    return {Intensity(mean_bkg, mean_bkg / nbkg), Intensity(mean_grad, sigma_grad)};
}

} // namespace


PixelSumIntegrator::PixelSumIntegrator(bool fit_center, bool fit_covariance) : IIntegrator()
{
    _params.fit_center = fit_center;
    _params.fit_cov = fit_covariance;
}

bool PixelSumIntegrator::compute(Peak3D* peak, ShapeModel*, const IntegrationRegion& region)
{
    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    if (events.empty()) {
        peak->setIntegrationFlag(RejectionFlag::TooFewPoints, IntegratorType::PixelSum);
        return false;
    }

    auto [meanBackground, bkgGradient] = compute_background(region, _params.use_gradient);
    if (!meanBackground.isValid()) {
        peak->setIntegrationFlag(RejectionFlag::TooFewPoints, IntegratorType::PixelSum);
        return false;
    }
    _sumBackground = meanBackground;
    _meanBkgGradient = bkgGradient;

    PeakCoordinateSystem frame(peak);

    double sum_peak = 0.0;
    const double mean_bkg = _sumBackground.value();
    // note that this is the std of the _estimate_ of the background
    // should be approximately mean_bkg / num_bkg for Poisson statistics
    const double std_bkg = _sumBackground.sigma();
    size_t npeak = 0.0;
    size_t nbkg = 0.0;
    Blob3D blob;

    // background sigma, assuming Poisson statistics
    const double sigma = std::sqrt(mean_bkg);

    for (auto i = 0; i < counts.size(); ++i) {
        const auto& ev = events[i];
        auto ev_type = region.classify(ev);

        if (ev_type == IntegrationRegion::EventType::BACKGROUND) {
            nbkg++;
            continue;
        }

        if (ev_type == IntegrationRegion::EventType::PEAK) {
            sum_peak += counts[i];
            npeak++;

            // update blob if pixel is strong (Poisson statistics)
            if (counts[i] > mean_bkg + sigma)
                blob.addPoint(ev.px, ev.py, ev.frame, counts[i] - mean_bkg);
        }
    }

    sum_peak -= npeak * _sumBackground.value();

    // TODO: ERROR ESTIMATE!!
    // This INCORRECTLY assumes Poisson statistics (no gain or baseline)
    _sumIntensity =
        Intensity(sum_peak, sum_peak + npeak * mean_bkg + npeak * npeak * std_bkg * std_bkg);

    // TODO: compute rocking curve
    double f_min = int(events[0].frame);
    double f_max = f_min;

    for (size_t i = 0; i < counts.size(); ++i) {
        const auto& ev = events[i];
        f_min = std::min(ev.frame, f_min);
        f_max = std::max(ev.frame, f_max);
    }

    Eigen::Vector3d center;
    Eigen::Matrix3d cov;

    if (fitCenter()) {
        if (blob.isValid()) {
            center = blob.center();
        } else {
            peak->setIntegrationFlag(RejectionFlag::InvalidCentroid, IntegratorType::PixelSum);
            return false;
        }
    } else {
        center = peak->shape().center();
    }

    if (fitCov()) {
        if (blob.isValid()) {
            cov = blob.covariance();
        } else {
            peak->setIntegrationFlag(RejectionFlag::InvalidCentroid, IntegratorType::PixelSum);
            return false;
        }
    } else {
        cov = peak->shape().inverseMetric();
    }

    // center of mass is consistent
    if (std::isnan(center.norm())) {
        peak->setIntegrationFlag(RejectionFlag::InvalidCentroid, IntegratorType::PixelSum);
        return false;
    }

    if (!peak->shape().isInside(center)) {
        peak->setIntegrationFlag(RejectionFlag::InvalidCentroid, IntegratorType::PixelSum);
        return false;
    }

    Eigen::Matrix3d A0 = peak->shape().metric();
    Eigen::Matrix3d A1 = cov.inverse();
    const double dA = (A1 - A0).norm() / A0.norm();

    // check that the covariance is consistent
    if (!(dA < 2.0)) {
        peak->setIntegrationFlag(RejectionFlag::InvalidCovariance, IntegratorType::PixelSum);
        return false;
    }

    // shape is not too small or too large
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    const auto& w = solver.eigenvalues();
    if (w.minCoeff() < 0.1 || w.maxCoeff() > 100) {
        peak->setIntegrationFlag(RejectionFlag::InvalidShape, IntegratorType::PixelSum);
        return false;
    }

    peak->setShape(Ellipsoid(center, A1));

    size_t nframes = size_t(f_max - f_min) + 1;
    _rockingCurve.resize(nframes);

    std::vector<double> intensity_per_frame(nframes, 0.0);
    std::vector<double> n_peak_points_per_frame(nframes, 0.0);
    std::vector<double> n_bkg_points_per_frame(nframes, 0.0);

    for (auto i = 0; i < counts.size(); ++i) {
        const auto& ev = events[i];
        const auto& ev_type = region.classify(ev);

        const int bin = ev.frame - f_min;

        if (ev_type == IntegrationRegion::EventType::PEAK) {
            intensity_per_frame[bin] += counts[i];
            n_peak_points_per_frame[bin] += 1;
        } else if (ev_type == IntegrationRegion::EventType::BACKGROUND)
            n_bkg_points_per_frame[bin] += 1;
    }

    for (int i = 0; i < nframes; ++i) {
        const double corrected_intensity =
            intensity_per_frame[i] - n_peak_points_per_frame[i] * mean_bkg;
        _rockingCurve[i] = Intensity(corrected_intensity, sqrt(corrected_intensity));
    }
    _profileIntensity = {};
    _profileBackground = {};

    return true;
}

} // namespace ohkl
