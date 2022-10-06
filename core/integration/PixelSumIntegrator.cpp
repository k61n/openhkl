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
#include "core/data/DataSet.h"
#include "core/integration/Blob3D.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"

namespace ohkl {

namespace {

std::pair<bool,Intensity> compute_background(const IntegrationRegion& region)
{
    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    if (events.size() < 20) {
        ohklLog(Level::Debug, "compute_background: too few data points");
        return {false, {}};
    }

    // Compute mean and variance. Repeat until no more outliers are to be rejected.
    double mean_bkg = 0;
    double var_bkg;
    double sigma_bkg;
    size_t nbkg;
    for (auto iteration = 0; iteration < 20; ++iteration) {
        double sum_bkg = 0;
        double sum_bkg2 = 0;
        nbkg = 0;
        for (auto i = 0; i < counts.size(); ++i) {
            if (region.classify(events[i]) != IntegrationRegion::EventType::BACKGROUND)
                continue;
            if (iteration>0 && std::fabs(counts[i] - mean_bkg) > 3 * sigma_bkg)
                continue;
            sum_bkg += counts[i];
            sum_bkg2 += counts[i] * counts[i];
            nbkg++;
        }
        double old_mean = mean_bkg;
        mean_bkg = sum_bkg / nbkg;
        var_bkg = (sum_bkg2 - nbkg * mean_bkg * mean_bkg) / (nbkg - 1);
        sigma_bkg = std::sqrt(var_bkg);
        if (iteration > 0 && std::fabs((old_mean - mean_bkg) / mean_bkg) < 1e-9)
            break;
    }

    return {true, Intensity(mean_bkg, mean_bkg / nbkg)};
}

} // namespace


PixelSumIntegrator::PixelSumIntegrator(bool fit_center, bool fit_covariance)
    : IIntegrator()
{
    _params.fit_center = fit_center;
    _params.fit_cov = fit_covariance;
}

bool PixelSumIntegrator::compute(Peak3D* peak, ShapeModel*, const IntegrationRegion& region)
{
    auto [ok, meanBackground] = compute_background(region);
    if (!ok) {
        peak->setRejectionFlag(RejectionFlag::TooFewPoints);
        peak->setSelected(false);
        return false;
    }
    _meanBackground = meanBackground;

    PeakCoordinateSystem frame(peak);

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    double sum_peak = 0.0;
    const double mean_bkg = _meanBackground.value();
    // note that this is the std of the _estimate_ of the background
    // should be approximately mean_bkg / num_bkg for Poisson statistics
    const double std_bkg = _meanBackground.sigma();
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

    sum_peak -= npeak * _meanBackground.value();

    // TODO: ERROR ESTIMATE!!
    // This INCORRECTLY assumes Poisson statistics (no gain or baseline)
    _integratedIntensity =
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
            peak->setRejectionFlag(RejectionFlag::InvalidCentroid);
            peak->setSelected(false);
            return false;
        }
    } else {
        center = peak->shape().center();
    }

    if (fitCov()) {
        if (blob.isValid()) {
            cov = blob.covariance();
        } else {
            peak->setRejectionFlag(RejectionFlag::InvalidCentroid);
            peak->setSelected(false);
            return false;
        }
    } else {
        cov = peak->shape().inverseMetric();
    }

    // center of mass is consistent
    if (std::isnan(center.norm())) {
        peak->setRejectionFlag(RejectionFlag::InvalidCentroid);
        peak->setSelected(false);
        return false;
    }

    if (!peak->shape().isInside(center)) {
        peak->setRejectionFlag(RejectionFlag::InvalidCentroid);
        peak->setSelected(false);
        return false;
    }

    Eigen::Matrix3d A0 = peak->shape().metric();
    Eigen::Matrix3d A1 = cov.inverse();
    const double dA = (A1 - A0).norm() / A0.norm();

    // check that the covariance is consistent
    if (!(dA < 2.0)) {
        peak->setRejectionFlag(RejectionFlag::InvalidCovariance);
        peak->setSelected(false);
        return false;
    }

    // shape is not too small or too large
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    const auto& w = solver.eigenvalues();
    if (w.minCoeff() < 0.1 || w.maxCoeff() > 100) {
        peak->setRejectionFlag(RejectionFlag::InvalidShape);
        peak->setSelected(false);
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

    return true;
}

} // namespace ohkl
