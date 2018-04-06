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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "DataSet.h"
#include "Ellipsoid.h"
#include "FitParameters.h"
#include "Intensity.h"
#include "Minimizer.h"
#include "Peak3D.h"
#include "GaussianIntegrator.h"

#include <Eigen/Cholesky>


namespace nsx {

GaussianIntegrator::GaussianIntegrator(bool fit_center, bool fit_cov): IPeakIntegrator(), _fitCenter(fit_center), _fitCov(fit_cov)
{

}

static Eigen::Matrix3d from_cholesky(const Eigen::VectorXd a)
{
    // Reconstruct Cholesky L factor
    Eigen::Matrix3d L;
    L.setZero();
    L(0,0) = a(0); L(1,1) = a(1); L(2,2) = a(2);
    L(1,0) = a(3); L(2,0) = a(4); L(2,1) = a(5);
    // build A using Cholesky decomposition
    return L*L.transpose();
}


static void residuals(
    Eigen::VectorXd& res,  
    double B, double I, 
    const Eigen::Vector3d x0, const Eigen::VectorXd& a, 
    const std::vector<Eigen::Vector3d>& x, const std::vector<double>& M)
{
    const size_t n = x.size();
    assert(n == M.size());
    assert(n == res.size());

    const Eigen::Matrix3d A = from_cholesky(a);
    const double factor = std::sqrt(A.determinant() / 8 / M_PI / M_PI / M_PI);

    for (size_t i = 0; i < n; ++i) {
        Eigen::Vector3d dx = x[i] - x0;
        const double xAx = dx.dot(A*dx);
        res[i] = B+I*std::exp(-0.5*xAx)*factor - M[i];
    }
}

bool GaussianIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    if (!peak) {
        return false;
    }

    const size_t N = region.data().events().size();
    
    std::vector<double> counts(N);
    std::vector<Eigen::Vector3d> x(N);
    Eigen::VectorXd wts(N);

    for (size_t i = 0; i < N; ++i) {
        counts[i] = region.data().counts()[i];
        const auto& ev = region.data().events()[i];
        x[i] = {ev._px, ev._py, ev._frame};
        wts[i] = counts[i] <= 0.0 ? 0.0 : 1.0/counts[i];
    }

    const auto& shape = peak->getShape();
    Eigen::Vector3d x0 = shape.center();

    // We only fit independent components of the Cholesky factor
    Eigen::Matrix3d L = Eigen::LLT<Eigen::Matrix3d>(shape.metric()).matrixL();
    Eigen::VectorXd a(6);
    a(0) = L(0,0); a(1) = L(1,1); a(2) = L(2,2);
    a(3) = L(1,0); a(4) = L(2,0); a(5) = L(2,1);
   
    Minimizer min;
    FitParameters params;

    double B = 0.0;
    double I = peak->getRawIntensity().value();

    params.addParameter(&B);
    params.addParameter(&I);

    if (_fitCenter) {
        for (size_t i = 0; i < 3; ++i) {
            params.addParameter(&x0(i));
        }
    }

    if (_fitCov) {
        for (size_t i = 0; i < 6; ++i) {
            params.addParameter(&a(i));
        }
    }

    auto f = [&](Eigen::VectorXd& r) -> int {
        residuals(r, B, I, x0, a, x, counts);
        return 0;
    };

    params.resetConstraints();

    min.initialize(params, N);
    min.set_f(f);

    min.setWeights(wts);

    bool success = min.fit(100);

    if (!success) {
        return false;
    }

    const auto& covar = min.covariance();
    _meanBackground = {B, covar(0,0)};
    _integratedIntensity = {I, covar(1,1)};

    // update peak shape
    peak->setShape({x0, from_cholesky(a)});

    // TODO: rocking curve!

    return true;
}

std::vector<double> GaussianIntegrator::profile(sptrPeak3D peak, const IntegrationRegion& region)
{
    const auto& events = region.data().events();
    const Eigen::Matrix3d A = peak->getShape().metric();
    const Eigen::Vector3d x0 = peak->getShape().center();
    const double factor = std::sqrt(A.determinant() / 8.0 / M_PI / M_PI / M_PI);
    std::vector<double> result(events.size(), 0.0);

    for (size_t i = 0; i < events.size(); ++i) {
        const DetectorEvent& ev = events[i];
        Eigen::Vector3d dx(ev._px, ev._py, ev._frame);
        dx -= x0;
        result[i] = std::exp(-0.5 * dx.dot(A*dx)) * factor;
    }
    return result;
}

} // end namespace nsx
