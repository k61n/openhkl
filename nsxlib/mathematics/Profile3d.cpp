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

#include "../mathematics/Profile3d.h"
#include "../mathematics/Minimizer.h"
#include "../utils/FitParameters.h"

#include <iostream>

namespace nsx {

Profile3d::Profile3d(double background, double A, const Eigen::Vector3d& c, const Eigen::Matrix3d& CI):
    _background(background), _A(A), _c(c),
    _Dxx(CI(0,0)),_Dxy(CI(0,1)),_Dxz(CI(0,2)),_Dyy(CI(1,1)),_Dyz(CI(1,2)),_Dzz(CI(2,2))
{

}


void Profile3d::evaluateInPlace(Eigen::VectorXd& result, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const
{
    const int n = result.size();
    assert(n == x.size());
    assert(n == y.size());
    assert(n == z.size());

    // get shifted coordinates
    auto dx = x - _c(0);
    auto dy = y - _c(1);
    auto dz = z - _c(2);

    // evaluate the argument of the exponential
    auto arg = -0.5*(_Dxx*dx*dx + _Dyy*dy*dy + _Dzz*dz*dz + 2*_Dxy*dx*dy + 2*_Dxz*dx*dz + 2*_Dyz*dy*dz);
    result.array() = _background + _A * arg.exp();
}

// note: the ordering of the columns of J corresponds to the order in which parameters are added to FitParameters in Profile3d::fit().
void Profile3d::jacobianInPlace(Eigen::MatrixXd& J, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const
{
    const int n = J.rows();
    assert(n == x.size());
    assert(n == y.size());
    assert(n == z.size());   


    // get shifted coordinates
    auto dx = x - _c(0);
    auto dy = y - _c(1);
    auto dz = z - _c(2);

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
    J.col(2).array() = _A*e * (_Dxx*dx + _Dxy*dy + _Dxz*dz);

    // derivative wrt y0
    J.col(3).array() = _A*e * (_Dyy*dy + _Dxy*dx + _Dyz*dz);

    // derivative wrt z0
    J.col(4).array() = _A*e * (_Dzz*dz + _Dxz*dx + _Dyz*dy);

    // derivative wrt dxx
    J.col(5).array() = -0.5*_A*e*dxdx;

    // derivative wrt dxy
    J.col(6).array() = -_A*e*dxdy;

    // derivative wrt dxz
    J.col(7).array() = -_A*e*dxdz;

    // derivative wrt dyy
    J.col(8).array() = -0.5*_A*e*dydy;

    // derivative wrt dyz
    J.col(9).array() = -_A*e*dydz;

    // derivative wrt dzz
    J.col(10).array() = -0.5*_A*e*dzdz;        
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


Profile3d::Profile3d(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I)
{
    const int nvalues = I.size();

    assert(x.size() == nvalues);
    assert(y.size() == nvalues);
    assert(z.size() == nvalues);

    _background = I.mean();
    const double avg_bkg = I.mean();
    const double std_bkg = (I-_background).sum() / (I.size()-1);

    Eigen::ArrayXd strong_I = (I-avg_bkg);

    for (int i = 0; i < nvalues; ++i) {
        if (strong_I(i) < std_bkg) {
             strong_I(i) = 0.0;
        }
    }

    _A = strong_I.maxCoeff();
    // estimate total mass of the profile
    const double mass = (strong_I).sum();

    // normalized mass distribution
    auto rho = strong_I / mass;

    // compute center of mass using distribution rho
    _c(0) = (x*rho).sum();
    _c(1) = (y*rho).sum();
    _c(2) = (z*rho).sum();

    // shifted coordinates
    auto dx = x-_c(0);
    auto dy = y-_c(1);
    auto dz = z-_c(2);
    
    // covariance matrix
    double c00 = (dx*dx*rho).sum();
    double c01 = (dx*dy*rho).sum();
    double c02 = (dx*dz*rho).sum();
    double c11 = (dy*dy*rho).sum();
    double c12 = (dy*dz*rho).sum();
    double c22 = (dz*dz*rho).sum();

    Eigen::Matrix3d C;
    C << c00, c01, c02, c01, c11, c12, c02, c12, c22;
    Eigen::Matrix3d CI = C.inverse();
    _Dxx = CI(0,0);
    _Dxy = CI(0,1);
    _Dxz = CI(0,2);
    _Dyy = CI(1,1);
    _Dyz = CI(1,2);
    _Dzz = CI(2,2);
}

Eigen::ArrayXd Profile3d::evaluate(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const
{
    const int n = x.size();
    assert(y.size() == n);
    assert(z.size() == n);

    Eigen::VectorXd result(n);
    evaluateInPlace(result, x, y, z);
    return result.array();
}

Profile3d Profile3d::fit(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I, int maxiter) const
{
    const int nvalues = I.size();
    assert(x.size() == nvalues);
    assert(y.size() == nvalues);
    assert(z.size() == nvalues);

    Profile3d result(*this);
    FitParameters params;
    // note: the order in which we add parameters matters, because we have to get the 
    // correct indicies in the Jacobian calculation!
    params.addParameter(&result._background);
    params.addParameter(&result._A);
    params.addParameter(&result._c(0));
    params.addParameter(&result._c(1));
    params.addParameter(&result._c(2));
    params.addParameter(&result._Dxx);
    params.addParameter(&result._Dxy);
    params.addParameter(&result._Dxz);
    params.addParameter(&result._Dyy);
    params.addParameter(&result._Dyz);
    params.addParameter(&result._Dzz);
    
    auto f = [&](Eigen::VectorXd& residual) -> int
    {
        // compute profile with given parameters
        result.evaluateInPlace(residual, x, y, z);
        // subtract observed intensity to get residuals
        residual -= I.matrix();
        return 0;
    };

    auto df = [&](Eigen::MatrixXd& J) -> int
    {
        result.jacobianInPlace(J, x, y, z);
        return 0;
    };
 
    Minimizer min;
    min.initialize(params, nvalues);
    min.setWeights(I.sqrt().array());
    min.set_f(f);
    min.set_df(df);

    bool success = min.fit(maxiter);

    Eigen::ArrayXd result_profile = result.evaluate(x, y, z);
    result._success = success;
    result._pearson = pearson_helper(result_profile, I);

    return result;
}

bool Profile3d::success() const
{
    return _success;
}

double Profile3d::pearson() const
{
    return _pearson;
}

double Profile3d::evaluate(Eigen::Vector3d p) const
{
    Eigen::ArrayXd x(1), y(1), z(1);
    Eigen::VectorXd result(1);
    x(0) = p(0);
    y(0) = p(1);
    z(0) = p(2);
    evaluateInPlace(result, x, y, z);
    return result(0);
}

} // end namespace nsx
