//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/Profile1DIntegrator.cpp
//! @brief     Implements class Profile1DIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/Profile1DIntegrator.h"
#include "base/geometry/Ellipsoid.h"
#include "core/data/DataSet.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/ShapeModel.h"

namespace nsx {

Profile1DIntegrator::Profile1DIntegrator() : IPeakIntegrator() { }

static void updateFit(
    Intensity& I, Intensity& B, const std::vector<double>& dp, const std::vector<double>& dM,
    const std::vector<int>& dn)
{
    Eigen::Matrix2d A;
    A.setZero();
    Eigen::Vector2d b(0, 0);
    const size_t n = dp.size();
    assert(dp.size() == dM.size() && dp.size() == dn.size());

    Eigen::Matrix2d b_cov;
    b_cov.setZero();

    for (size_t i = 0; i < n; ++i) {
        // avoid case where dn[i] = 0
        if (dn[i] == 0)
            continue;

        const double p = dp[i];
        const double n = dn[i];
        const double M = dM[i];
        const double var = B.value() * dn[i] + I.value() * dp[i];

        A(0, 0) += n * n / var;
        A(0, 1) += n * p / var;
        A(1, 0) += n * p / var;
        A(1, 1) += p * p / var;

        b(0) += M * n / var;
        b(1) += M * p / var;

        b_cov(0, 0) += n * n / var;
        b_cov(1, 0) += p * n / var;
        b_cov(0, 1) += p * n / var;
        b_cov(1, 1) += p * p / var;
    }

    Eigen::Matrix2d AI = A.inverse();
    const Eigen::Vector2d& x = AI * b;

    const double new_B = x(0);
    const double new_I = x(1);

    Eigen::Matrix2d cov = AI * b_cov * AI.transpose();

    // Note: this error estimate assumes the variances are correct (i.e., gain and
    // baseline accounted for)
    B = Intensity(new_B, cov(0, 0));
    I = Intensity(new_I, cov(1, 1));
}

bool Profile1DIntegrator::compute(
    Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region)
{
    if (!shape_model) {
        peak->setRejectionFlag(RejectionFlag::NoShapeModel);
        peak->setSelected(false);
        return false;
    }

    if (!peak)
        return false;

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    // TODO: should this be hard-coded??
    if (events.size() < 29) {
        peak->setRejectionFlag(RejectionFlag::TooFewPoints);
        peak->setSelected(false);
        return false;
    }


    Eigen::Vector3d c = peak->shape().center();
    Eigen::Matrix3d A = peak->shape().metric();

    Profile1D profile(0.0, region.peakEnd());
    std::optional<std::vector<Intensity>> mean_profile =
        shape_model->meanProfile1D(DetectorEvent(c), radius(), nFrames());
    if (!mean_profile)
        return false;

    // construct the observed profile
    for (size_t i = 0; i < events.size(); ++i) {
        Eigen::Vector3d dx(events[i].px, events[i].py, events[i].frame);
        dx -= c;
        const double r2 = dx.transpose() * A * dx;
        profile.addPoint(r2, counts[i]);
    }

    std::vector<int> dn;
    std::vector<double> dm;
    std::vector<double> dp;

    dn.push_back(profile.npoints()[0]);
    dm.push_back(profile.counts()[0]);
    dp.push_back(mean_profile.value()[0].value());

    // compute differences and rebin if necessary so that dn > 0
    for (size_t i = 1; i < mean_profile.value().size(); ++i) {
        const auto& counts = profile.counts();
        const auto& npoints = profile.npoints();
        dn.push_back(npoints[i] - npoints[i - 1]);
        dm.push_back(counts[i] - counts[i - 1]);
        dp.push_back(mean_profile.value()[i].value() - mean_profile.value()[i - 1].value());
    }

    Intensity I = 1e-6;
    Intensity B = 1.0;

    for (auto i = 0; i < 10 && I.value() > 0; ++i)
        updateFit(I, B, dp, dm, dn);

    double sigma = I.sigma();

    if (std::isnan(sigma) || sigma <= 0.0) {
        peak->setRejectionFlag(RejectionFlag::InvalidSigma);
        peak->setSelected(false);
        return false;
    }

    _integratedIntensity = I;
    _meanBackground = B;

    // TODO: rocking curve!

    return true;
}

} // namespace nsx
