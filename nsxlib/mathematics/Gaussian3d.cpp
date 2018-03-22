/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
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

#include "FitParameters.h"
#include "Minimizer.h"
#include "Gaussian3d.h"

namespace nsx {

Gaussian3d::Gaussian3d(double background, double max, const Eigen::Vector3d& center, const Eigen::Matrix3d& covariance):
    _background(background), _max(max), _center(center)
{
    Eigen::Matrix3d D = covariance.inverse();
    _Dxx = D(0,0);
    _Dxy = D(0,1);
    _Dxz = D(0,2);
    _Dyy = D(1,1);
    _Dyz = D(1,2);
    _Dzz = D(2,2);
}


void Gaussian3d::evaluateInPlace(Eigen::VectorXd& result, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const
{
    // get shifted coordinates
    auto dx = x - _center(0);
    auto dy = y - _center(1);
    auto dz = z - _center(2);

    // evaluate the argument of the exponential
    auto arg = -0.5*(_Dxx*dx*dx + _Dyy*dy*dy + _Dzz*dz*dz + 2*_Dxy*dx*dy + 2*_Dxz*dx*dz + 2*_Dyz*dy*dz);
    result.array() = _background + _max * arg.exp();
}

// note: the ordering of the columns of J corresponds to the order in which parameters are added to FitParameters in Gaussian3d::fit().
void Gaussian3d::jacobianInPlace(Eigen::MatrixXd& J, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const
{
    // get shifted coordinates
    auto dx = x - _center(0);
    auto dy = y - _center(1);
    auto dz = z - _center(2);

    auto dxdx = dx*dx;
    auto dxdy = dx*dy;
    auto dxdz = dx*dz;
    auto dydy = dy*dy;
    auto dydz = dy*dz;
    auto dzdz = dz*dz;

    // evaluate the argument of the exponential
    auto arg = -0.5*(_Dxx*dxdx + _Dyy*dydy + _Dzz*dzdz + 2*_Dxy*dxdy + 2*_Dxz*dxdz + 2*_Dyz*dydz);
    // exponential
    auto e = arg.exp();

    // derivative wrt background
    J.col(0).array() = 1;

    // derivative wrt A
    J.col(1).array() = e;

    // derivative wrt x0
    J.col(2).array() = _max*e * (_Dxx*dx + _Dxy*dy + _Dxz*dz);

    // derivative wrt y0
    J.col(3).array() = _max*e * (_Dyy*dy + _Dxy*dx + _Dyz*dz);

    // derivative wrt z0
    J.col(4).array() = _max*e * (_Dzz*dz + _Dxz*dx + _Dyz*dy);

    // derivative wrt dxx
    J.col(5).array() = -0.5*_max*e*dxdx;

    // derivative wrt dxy
    J.col(6).array() = -_max*e*dxdy;

    // derivative wrt dxz
    J.col(7).array() = -_max*e*dxdz;

    // derivative wrt dyy
    J.col(8).array() = -0.5*_max*e*dydy;

    // derivative wrt dyz
    J.col(9).array() = -_max*e*dydz;

    // derivative wrt dzz
    J.col(10).array() = -0.5*_max*e*dzdz;        
}

double pearson_helper(const Eigen::ArrayXd& pred, const Eigen::ArrayXd& obs)
{
    const double mu_obs = obs.mean();
    auto dobs = obs-mu_obs;
    const double std_obs = std::sqrt((dobs*dobs).mean());

    const double mu_pred = pred.mean();
    auto dpred = pred-mu_pred;
    const double std_pred = std::sqrt((dpred*dpred).mean());

    const double cov = (dobs*dpred).mean();
    return cov / (std_pred*std_obs);
}

Eigen::ArrayXd Gaussian3d::evaluate(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const
{
    const int n = x.size();
    assert(y.size() == n);
    assert(z.size() == n);

    Eigen::VectorXd result(n);
    evaluateInPlace(result, x, y, z);
    return result.array();
}

bool Gaussian3d::fit(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I, int maxiter)
{
    const int nvalues = I.size();
    assert(x.size() == nvalues);
    assert(y.size() == nvalues);
    assert(z.size() == nvalues);

    FitParameters params;
    // note: the order in which we add parameters matters, because we have to get the 
    // correct indicies in the Jacobian calculation!
    params.addParameter(&_background);
    params.addParameter(&_max);
    params.addParameter(&_center(0));
    params.addParameter(&_center(1));
    params.addParameter(&_center(2));
    params.addParameter(&_Dxx);
    params.addParameter(&_Dxy);
    params.addParameter(&_Dxz);
    params.addParameter(&_Dyy);
    params.addParameter(&_Dyz);
    params.addParameter(&_Dzz);
    
    auto f = [&](Eigen::VectorXd& residual) -> int
    {
        // compute profile with given parameters
        evaluateInPlace(residual, x, y, z);
        // subtract observed intensity to get residuals
        residual -= I.matrix();
        return 0;
    };

    auto df = [&](Eigen::MatrixXd& J) -> int
    {
        jacobianInPlace(J, x, y, z);
        return 0;
    };
 
    Minimizer min;
    min.initialize(params, nvalues);
    min.setWeights(I.sqrt().array());
    min.set_f(f);
    min.set_df(df);

    bool success = min.fit(maxiter);

    Eigen::ArrayXd result_profile = evaluate(x, y, z);
    _success = success;

    return success;
}

bool Gaussian3d::success() const
{
    return _success;
}

double Gaussian3d::evaluate(Eigen::Vector3d p) const
{
    Eigen::ArrayXd x(1), y(1), z(1);
    Eigen::VectorXd result(1);
    x(0) = p(0);
    y(0) = p(1);
    z(0) = p(2);
    evaluateInPlace(result, x, y, z);
    return result(0);
}

Eigen::Matrix3d Gaussian3d::covariance() const
{
    Eigen::Matrix3d D;
    D(0,0) = _Dxx;
    D(1,1) = _Dyy;
    D(2,2) = _Dzz;

    D(0,1) = D(1,0) = _Dxy;
    D(0,2) = D(2,0) = _Dxz;
    D(1,2) = D(2,1) = _Dyz;

    return D.inverse();
}

double Gaussian3d::pearson(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I) const
{
    Eigen::ArrayXd I_pred = evaluate(x, y, z);
    return pearson_helper(I_pred, I);
}

} // end namespace nsx
