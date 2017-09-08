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

#pragma once

#include <Eigen/Dense>


namespace nsx {

//! convenience data structure for converting between named parameters and flattened arrays
struct ProfileParams {
    static const int nparams;
    //! Background value
    double background;
    //! x component of center of mass
    double x0;
    double y0;
    double z0;
    double A;
    double dxx;
    double dxy;
    double dxz;
    double dyy;
    double dyz;
    double dzz;

    //! Default constructor
    ProfileParams() = default;
    //! Initialize by guessing parameters from the given data points
    ProfileParams(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I);

    Eigen::VectorXd pack() const
    {
        Eigen::VectorXd p(nparams);
        p << background, x0, y0, z0, A, dxx, dxy, dxz, dyy, dyz, dzz;
        return p;
    }

    void unpack(const Eigen::VectorXd& p)
    {
        assert(p.size() == nparams);
        background = p(0);
        x0 = p(1);
        y0 = p(2);
        z0 = p(3);
        A = p(4);
        dxx = p(5);
        dxy = p(6);
        dxz = p(7);
        dyy = p(8);
        dyz = p(9);
        dzz = p(10);
    }
};

//! Class for 3d peak profile fitting
class Profile3d {
public:
    //! Initialize with a given set of parameters
    Profile3d(const ProfileParams& p);
    //! Destructor
    ~Profile3d();

    //! Fit using the provided data points
    Profile3d fit(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I, int maxiter=100) const;

    //! Value of the profile at the given point (including background)
    double profile(Eigen::Vector3d p) const;

    //! Values of the profile at the given points (including background)
    Eigen::ArrayXd profile(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const;

    //! Pearson correlation coefficient of the fit
    double pearson() const;

    //! Return the named parameters of the profile
    ProfileParams parameters() const;

    //! Return whether this profile is the result of a successful fit
    bool success() const;

private:
    //! Parameters of the profile
    Eigen::VectorXd _params;
    //! Pearson correlation coefficient
    double _pearson;
    //! Flag to determine whether this profile is the result of a successful fit
    bool _success;
    //! Center of mass
    Eigen::Vector3d _center;
    //! Covariance matrix
    Eigen::Matrix3d _covariance;

};

} // end namespace nsx
