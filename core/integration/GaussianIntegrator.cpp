//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/GaussianIntegrator.cpp
//! @brief     Implements class GaussianIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/GaussianIntegrator.h"
#include "base/fit/FitParameters.h"
#include "base/fit/Minimizer.h"
#include "base/geometry/Ellipsoid.h"
#include "core/data/DataSet.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"

#include <Eigen/Cholesky>
#include <QDebug>
#include <QtGlobal>

namespace nsx {

GaussianIntegrator::GaussianIntegrator(bool fit_center, bool fit_cov)
{
    _params.fit_center = fit_center;
    _params.fit_cov = fit_cov;
}

static Eigen::Matrix3d from_cholesky(const Eigen::VectorXd a)
{
    // Reconstruct Cholesky L factor
    Eigen::Matrix3d L;
    L.setZero();
    L(0, 0) = a(0);
    L(1, 1) = a(1);
    L(2, 2) = a(2);
    L(1, 0) = a(3);
    L(2, 0) = a(4);
    L(2, 1) = a(5);
    // build A using Cholesky decomposition
    return L * L.transpose();
}

static void residuals(
    Eigen::VectorXd& res, double B, double I, const Eigen::Vector3d x0, const Eigen::VectorXd& a,
    const std::vector<Eigen::Vector3d>& x, const std::vector<double>& M, double* pearson)
{
    const size_t n = x.size();
    assert(n == M.size());
    assert(n == res.size());

    const Eigen::Matrix3d A = from_cholesky(a);

    double u = 0, v = 0, uu = 0, vv = 0, uv = 0;

    for (size_t i = 0; i < n; ++i) {
        Eigen::Vector3d dx = x[i] - x0;
        const double xAx = dx.dot(A * dx);
        const double M_pred = B + I * std::exp(-0.5 * xAx);
        const double M_obs = M[i];
        res[i] = M_pred - M_obs;

        if (pearson) {
            u += M_pred;
            uu += M_pred * M_pred;
            v += M_obs;
            vv += M_obs * M_obs;
            uv += M_pred * M_obs;
        }
    }

    // pearson correlation
    if (pearson) {
        u /= n;
        v /= n;
        uu -= n * u * u;
        vv -= n * v * v;
        uv -= n * u * v;
        *pearson = uv / std::sqrt(uu * vv);
    }
}

bool GaussianIntegrator::compute(
    Peak3D* peak, ShapeCollection* /*shape_collection*/, const IntegrationRegion& region)
{
    if (!peak)
        return false;

    const size_t N = region.peakData().events().size();

    std::vector<double> counts(N);
    std::vector<Eigen::Vector3d> x(N);
    Eigen::VectorXd wts(N);

    for (size_t i = 0; i < N; ++i) {
        counts[i] = region.peakData().counts()[i];
        const auto& ev = region.peakData().events()[i];
        x[i] = {ev._px, ev._py, ev._frame};
        wts[i] = counts[i] <= 0.0 ? 0.0 : 1.0 / counts[i];
    }

    const auto& shape = peak->shape();
    Eigen::Vector3d x0 = shape.center();

    // We only fit independent components of the Cholesky factor
    Eigen::Matrix3d L = Eigen::LLT<Eigen::Matrix3d>(shape.metric()).matrixL();
    Eigen::VectorXd a(6);
    a(0) = L(0, 0);
    a(1) = L(1, 1);
    a(2) = L(2, 2);
    a(3) = L(1, 0);
    a(4) = L(2, 0);
    a(5) = L(2, 1);

    Minimizer min;
    FitParameters params;

    double B = 0.0;
    double I = peak->rawIntensity().value();

    params.addParameter(&B);
    params.addParameter(&I);

    if (fitCenter()) {
        for (size_t i = 0; i < 3; ++i)
            params.addParameter(&x0(i));
    }

    if (fitCov()) {
        for (size_t i = 0; i < 6; ++i)
            params.addParameter(&a(i));
    }

    auto f = [&](Eigen::VectorXd& r) -> int {
        residuals(r, B, I, x0, a, x, counts, nullptr);
        return 0;
    };

    params.resetConstraints();

    min.initialize(params, N);
    min.set_f(f);

    min.setWeights(wts);

    try {
        bool success = min.fit(100);
        if (!success)
            return false;
    } catch (std::exception& e) {
        qWarning() << "Gaussian fit failed: " << e.what();
        return false;
    }

    // consistency check: center should still be in dataset!
    if (x0(0) < 0 || x0(0) >= peak->dataSet()->nCols()) {
        peak->setRejectionFlag(RejectionFlag::CentreOutOfBounds);
        return false;
    }
    if (x0(1) < 0 || x0(1) >= peak->dataSet()->nRows()) {
        peak->setRejectionFlag(RejectionFlag::CentreOutOfBounds);
        return false;
    }
    if (x0(2) < 0 || x0(2) >= peak->dataSet()->nFrames()) {
        peak->setRejectionFlag(RejectionFlag::CentreOutOfBounds);
        return false;
    }

    // consistency check: covariance matrix should be positive definite
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(from_cholesky(a));

    if (solver.eigenvalues().minCoeff() <= 0) {
        peak->setRejectionFlag(RejectionFlag::InvalidCovariance);
        return false;
    }

    const auto& covar = min.covariance();
    _meanBackground = {B, covar(0, 0)};
    _integratedIntensity = {I, covar(1, 1)};

    // Gets pearson coefficient of fit
    double pearson;
    Eigen::VectorXd r(N);
    residuals(r, B, I, x0, a, x, counts, &pearson);

    if (pearson <= 0.75) {
        peak->setRejectionFlag(RejectionFlag::BadIntegrationFit);
        return false;
    }

    peak->setShape({x0, from_cholesky(a)});
    return true;
}

std::vector<double> GaussianIntegrator::profile(Peak3D* peak, const IntegrationRegion& region)
{
    const auto& events = region.peakData().events();
    const Eigen::Matrix3d A = peak->shape().metric();
    const Eigen::Vector3d x0 = peak->shape().center();
    const double factor = std::sqrt(A.determinant() / 8.0 / M_PI / M_PI / M_PI);

    std::vector<double> result(events.size(), 0.0);
    for (size_t i = 0; i < events.size(); ++i) {
        const DetectorEvent& ev = events[i];
        Eigen::Vector3d dx(ev._px, ev._py, ev._frame);
        dx -= x0;
        result[i] = std::exp(-0.5 * dx.dot(A * dx)) * factor;
    }
    return result;
}

} // namespace nsx
