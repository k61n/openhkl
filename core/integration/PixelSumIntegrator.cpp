/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
 *
 */

#include "PixelSumIntegrator.h"
#include "Blob3D.h"
#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "MeanBackgroundIntegrator.h"
#include "Peak3D.h"

namespace nsx {

PixelSumIntegrator::PixelSumIntegrator(bool fit_center, bool fit_covariance)
    : MeanBackgroundIntegrator(), _fitCenter(fit_center), _fitCovariance(fit_covariance)
{
}

PixelSumIntegrator::~PixelSumIntegrator() {}

bool PixelSumIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    MeanBackgroundIntegrator::compute(peak, region);
    PeakCoordinateSystem frame(peak);

    const auto& events = region.data().events();
    const auto& counts = region.data().counts();

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
            if (counts[i] > mean_bkg + sigma) {
                blob.addPoint(ev._px, ev._py, ev._frame, counts[i] - mean_bkg);
            }
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

    Eigen::Vector3d center = _fitCenter ? blob.center() : peak->shape().center();
    Eigen::Matrix3d cov = _fitCovariance ? blob.covariance() : peak->shape().inverseMetric();

    // center of mass is consistent
    if (std::isnan(center.norm())) {
        return false;
    }

    if (!peak->shape().isInside(center)) {
        return false;
    }

    Eigen::Matrix3d A0 = peak->shape().metric();
    Eigen::Matrix3d A1 = cov.inverse();
    const double dA = (A1 - A0).norm() / A0.norm();

    // check that the covariance is consistent
    if (!(dA < 2.0)) {
        return false;
    }

    // shape is not too small or too large
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    auto w = solver.eigenvalues();

    if (w.minCoeff() < 0.1 || w.maxCoeff() > 100) {
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
        auto ev_type = region.classify(ev);

        int bin = ev._frame - f_min;

        if (ev_type == IntegrationRegion::EventType::PEAK) {
            intensity_per_frame[bin] += counts[i];
            n_peak_points_per_frame[bin] += 1;
        } else if (ev_type == IntegrationRegion::EventType::BACKGROUND) {
            n_bkg_points_per_frame[bin] += 1;
        }
    }

    for (int i = 0; i < nframes; ++i) {
        double corrected_intensity = intensity_per_frame[i] - n_peak_points_per_frame[i] * mean_bkg;
        _rockingCurve[i] = Intensity(corrected_intensity, sqrt(corrected_intensity));
    }

    return true;
}

} // end namespace nsx
