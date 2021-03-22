//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/PixelSumIntegrator.cpp
//! @brief     Implements class PixelSumIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/PixelSumIntegrator.h"
#include "base/geometry/Ellipsoid.h"
#include "core/data/DataSet.h"
#include "core/integration/Blob3D.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"

namespace nsx {

PixelSumIntegrator::PixelSumIntegrator(bool fit_center, bool fit_covariance)
    : MeanBackgroundIntegrator()
{
    _params.fit_center = fit_center;
    _params.fit_cov = fit_covariance;
}

PixelSumIntegrator::~PixelSumIntegrator() = default;

bool PixelSumIntegrator::compute(
    Peak3D* peak, ShapeCollection* shape_collection, const IntegrationRegion& region)
{
    MeanBackgroundIntegrator::compute(peak, shape_collection, region);
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
                blob.addPoint(ev._px, ev._py, ev._frame, counts[i] - mean_bkg);
        }
    }

    sum_peak -= npeak * _meanBackground.value();

    // TODO: ERROR ESTIMATE!!
    // This INCORRECTLY assumes Poisson statistics (no gain or baseline)
    _integratedIntensity =
        Intensity(sum_peak, sum_peak + npeak * mean_bkg + npeak * npeak * std_bkg * std_bkg);

    // TODO: compute rocking curve
    double f_min = int(events[0]._frame);
    double f_max = f_min;

    for (size_t i = 0; i < counts.size(); ++i) {
        const auto& ev = events[i];
        f_min = std::min(ev._frame, f_min);
        f_max = std::max(ev._frame, f_max);
    }

    Eigen::Vector3d center = fitCenter() ? blob.center() : peak->shape().center();
    Eigen::Matrix3d cov = fitCov() ? blob.covariance() : peak->shape().inverseMetric();

    // center of mass is consistent
    if (std::isnan(center.norm()))
        return false;

    if (!peak->shape().isInside(center))
        return false;

    Eigen::Matrix3d A0 = peak->shape().metric();
    Eigen::Matrix3d A1 = cov.inverse();
    const double dA = (A1 - A0).norm() / A0.norm();

    // check that the covariance is consistent
    if (!(dA < 2.0))
        return false;

    // shape is not too small or too large
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    auto w = solver.eigenvalues();

    if (w.minCoeff() < 0.1 || w.maxCoeff() > 100)
        return false;

    peak->setShape(Ellipsoid(center, A1));

    size_t nframes = size_t(f_max - f_min) + 1;
    _rockingCurve.resize(nframes);

    std::vector<double> intensity_per_frame(nframes, 0.0);
    std::vector<double> n_peak_points_per_frame(nframes, 0.0);
    std::vector<double> n_bkg_points_per_frame(nframes, 0.0);

    for (auto i = 0; i < counts.size(); ++i) {
        const auto& ev = events[i];
        auto ev_type = region.classify(ev);

        int bin = ev._frame - f_min;

        if (ev_type == IntegrationRegion::EventType::PEAK) {
            intensity_per_frame[bin] += counts[i];
            n_peak_points_per_frame[bin] += 1;
        } else if (ev_type == IntegrationRegion::EventType::BACKGROUND)
            n_bkg_points_per_frame[bin] += 1;
    }

    for (int i = 0; i < nframes; ++i) {
        double corrected_intensity = intensity_per_frame[i] - n_peak_points_per_frame[i] * mean_bkg;
        _rockingCurve[i] = Intensity(corrected_intensity, sqrt(corrected_intensity));
    }

    return true;
}

} // namespace nsx
