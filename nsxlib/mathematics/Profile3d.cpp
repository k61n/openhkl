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

#include <iostream>

// anonymous namespace for helper structures and routines
namespace {

// convenience routine for computing the 3d profile. 
// The return value is passed by non-const reference because this is what the minimizer expects.
void profile_helper(Eigen::VectorXd& result, const Eigen::VectorXd& params, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z)
{
    const int n = result.size();
    assert(n == x.size());
    assert(n == y.size());
    assert(n == z.size());

    // get the named parameters
    nsx::ProfileParams p;
    p.unpack(params);

    // get shifted coordinates
    auto dx = x - p.x0;
    auto dy = y - p.y0;
    auto dz = z - p.z0;

    // evaluate the argument of the exponential
    auto arg = -0.5*(p.dxx*dx*dx + p.dyy*dy*dy + p.dzz*dz*dz + 2*p.dxy*dx*dy + 2*p.dxz*dx*dz + 2*p.dyz*dy*dz);
    result.array() = p.background + p.A * arg.exp();
}

// convenience routine for computing the Jacobian of the 3d profile. 
// The return value is passed by non-const reference because this is what the minimizer expects.
void jacobian_helper(Eigen::MatrixXd& result, const Eigen::VectorXd& params, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z)
{
    const int n = result.rows();

    assert(result.cols() == params.size());
    assert(n == x.size());
    assert(n == y.size());
    assert(n == z.size());   

    // get the named parameters
    nsx::ProfileParams p;
    p.unpack(params);

    // get shifted coordinates
    auto dx = x - p.x0;
    auto dy = y - p.y0;
    auto dz = z - p.z0;

    auto dxdx = dx*dx;
    auto dxdy = dx*dy;
    auto dxdz = dx*dz;
    auto dydy = dy*dy;
    auto dydz = dy*dz;
    auto dzdz = dz*dz;

    // evaluate the argument of the exponential
    auto arg = -0.5*(p.dxx*dxdx + p.dyy*dydy + p.dzz*dzdz + 2*p.dxy*dxdy + 2*p.dxz*dxdz + 2*p.dyz*dydz);
    // exponential
    auto e = arg.exp();

    // derivative wrt background
    result.col(0).array() = 1;

    // derivative wrt x0
    result.col(1).array() = p.A*e * (p.dxx*dx + p.dxy*dy + p.dxz*dz);

    // derivative wrt y0
    result.col(2).array() = p.A*e * (p.dyy*dy + p.dxy*dx + p.dyz*dz);

    // derivative wrt z0
    result.col(3).array() = p.A*e * (p.dzz*dz + p.dxz*dx + p.dyz*dy);

    // derivative wrt A
    result.col(4).array() = e;

    // derivative wrt dxx
    result.col(5).array() = -0.5*p.A*e*dxdx;

    // derivative wrt dxy
    result.col(6).array() = -p.A*e*dxdy;

    // derivative wrt dxz
    result.col(7).array() = -p.A*e*dxdz;

    // derivative wrt dyy
    result.col(8).array() = -0.5*p.A*e*dydy;

    // derivative wrt dyz
    result.col(9).array() = -p.A*e*dydz;

    // derivative wrt dzz
    result.col(10).array() = -0.5*p.A*e*dzdz;        
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

} // end anonymous namespace

namespace nsx {

const int ProfileParams::nparams = 11;

Profile3d::Profile3d(const ProfileParams& p): _params(p.nparams)
{
    _params = p.pack();
}

Profile3d::~Profile3d()
{

}

ProfileParams::ProfileParams(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I)
{
    const int nvalues = I.size();

    assert(x.size() == nvalues);
    assert(y.size() == nvalues);
    assert(z.size() == nvalues);

    background = I.mean();

    const double avg_bkg = I.mean();
    const double std_bkg = (I-background).sum() / (I.size()-1);

    Eigen::ArrayXd strong_I = (I-avg_bkg);

    for (int i = 0; i < nvalues; ++i) {
        if (strong_I(i) < std_bkg) {
             strong_I(i) = 0.0;
        }
    }

    A = strong_I.maxCoeff();
    // estimate total mass of the profile
    const double mass = (strong_I).sum();

    // normalized mass distribution
    auto rho = strong_I / mass;

    // compute center of mass using distribution rho
    x0 = (x*rho).sum();
    y0 = (y*rho).sum();
    z0 = (z*rho).sum();

    // shifted coordinates
    auto dx = x-x0;
    auto dy = y-y0;
    auto dz = z-z0;
    
    // covariance matrix
    double c00 = (dx*dx*rho).sum();
    double c01 = (dx*dy*rho).sum();
    double c02 = (dx*dz*rho).sum();
    double c11 = (dy*dy*rho).sum();
    double c12 = (dy*dz*rho).sum();
    double c22 = (dz*dz*rho).sum();

    Eigen::Matrix3d C;
    C << c00, c01, c02, c01, c11, c12, c02, c12, c22;
    Eigen::Matrix3d D = C.inverse();

    // coefficient of inverse covariance matrix
    dxx = D(0,0);
    dxy = D(0,1);
    dxz = D(0,2);
    dyy = D(1,1);
    dyz = D(1,2);
    dzz = D(2,2);
}

Eigen::ArrayXd Profile3d::profile(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const
{
    const int n = x.size();
    assert(y.size() == n);
    assert(z.size() == n);

    Eigen::VectorXd result(n);
    profile_helper(result, _params, x, y, z);
    return result.array();
}

Profile3d Profile3d::fit(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I, int maxiter) const
{
    const int nparams = _params.size();
    const int nvalues = I.size();
    assert(x.size() == nvalues);
    assert(y.size() == nvalues);
    assert(z.size() == nvalues);

    auto f = [&](const Eigen::VectorXd& params, Eigen::VectorXd& result) -> int
    {
        // compute profile with given parameters
        profile_helper(result, params.array(), x, y, z);
        // subtract observed intensity to get residuals
        result -= I.matrix();
        return 0;
    };

    auto df = [&](const Eigen::VectorXd& params, Eigen::MatrixXd& result) -> int
    {
        jacobian_helper(result, params.array(), x, y, z);
        return 0;
    };
 
    Minimizer min;
    min.initialize(nparams, nvalues);
    min.setParams(_params);
    min.setWeights(I.sqrt().array());
    min.set_f(f);
    min.set_df(df);
    bool success = min.fit(maxiter);

    ProfileParams p;
    p.unpack(min.params());
    Profile3d result(p);

    Eigen::ArrayXd result_profile = result.profile(x, y, z);

    result._success = success;
    result._pearson = pearson_helper(result_profile, I);

    return result;
}


ProfileParams Profile3d::parameters() const
{
    ProfileParams p;
    p.unpack(_params);
    return p;
}

bool Profile3d::success() const
{
    return _success;
}

double Profile3d::pearson() const
{
    return _pearson;
}

double Profile3d::profile(Eigen::Vector3d p) const
{
    Eigen::ArrayXd x(1), y(1), z(1);
    Eigen::VectorXd result(1);
    x(0) = p(0);
    y(0) = p(1);
    z(0) = p(2);
    profile_helper(result, _params, x, y, z);
    return result(0);
}

} // end namespace nsx
