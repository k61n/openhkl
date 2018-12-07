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

#include <vector>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace nsx {

//! \class FitParameters
//! \brief Class to encapsulate parameters to be passed to nsx::Minimizer.
class FitParameters {

public:
    //! Add a parameter to be fit. The original value of the parameter is stored (see reset()). The return value is the index of the parameter.
    int addParameter(double* addr);
    //! Set values of the parameters from a GSL vector.
    void setValues(const gsl_vector* v);
    //! Write the values to a GSL vector
    void writeValues(gsl_vector* v) const;
    //! Return the number of parameters
    size_t nparams() const;
    //! Return the number of free parameters
    size_t nfree() const;
    //! Set the constraint matrix
    void setConstraint(const Eigen::SparseMatrix<double>& C);
    //! Remove the constraints
    void resetConstraints();
    //! Kernel of the constraints: this is the transformation from constrained parameters to original parameters.
    const Eigen::MatrixXd& kernel() const;
    //! Set the kernel of the constraints.
    void setKernel(const Eigen::MatrixXd& ker);

    //! Reset parameter values to their original state.
    void reset();

private:
    //! Vector of addresses to fit parameters. Pointers are _not_ owned.
    std::vector<double*> _params;
    std::vector<double> _originalValues;
    //! Linear transformation from kernel to parameters. The columns of K form a basis
    //! for the kernel of _C.
    Eigen::MatrixXd _K;

    //! Projection from unconstrained parameter space to constrained parameter space.
    Eigen::MatrixXd _P;

    //! Need to update constraints
    bool _constrained;

};

} // end namespace nsx
