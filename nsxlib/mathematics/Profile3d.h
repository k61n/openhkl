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

//! Class for 3d peak profile fitting
class Profile3d {
public: 
    //! Create a profile with given initial parameters
    Profile3d(double background, double A, const Eigen::Vector3d& c, const Eigen::Matrix3d& CI);
    //! Create a profile with an initial guess calculated (not fit) from the given data.
    Profile3d(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I);
    //! Fit using the provided data points
    Profile3d fit(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z, const Eigen::ArrayXd& I, int maxiter=100) const;

    //! Value of the profile at the given point (including background)
    double evaluate(Eigen::Vector3d p) const;

    //! Values of the profile at the given points (including background)
    Eigen::ArrayXd evaluate(const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const;

    //! Pearson correlation coefficient of the fit
    double pearson() const;

    //! Return whether this profile is the result of a successful fit
    bool success() const;

private:
    //! Evaluate profile in place, assuming result has already been allocated
    void evaluateInPlace(Eigen::VectorXd& result, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const;
    //! Evaluate the Jacobian in place, assuming J has already been allocated
    void jacobianInPlace(Eigen::MatrixXd& J, const Eigen::ArrayXd& x, const Eigen::ArrayXd& y, const Eigen::ArrayXd& z) const;

    //! Pearson correlation coefficient
    double _pearson;
    //! Flag to determine whether this profile is the result of a successful fit
    bool _success;
    //! Center of mass
    Eigen::Vector3d _center;
    //! Covariance matrix
    Eigen::Matrix3d _covariance;

public:
    //! Background value
    double _background;
    //! Amplitude
    double _A;
    //! center of mass
    Eigen::Vector3d _c;
    //! Components of inverse covariance matrix. 
    double _Dxx, _Dxy, _Dxz, _Dyy, _Dyz, _Dzz;

};

} // end namespace nsx
