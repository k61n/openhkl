//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/fit/FitParameters.h
//! @brief     Defines class FitParameters
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_FIT_FITPARAMETERS_H
#define NSX_BASE_FIT_FITPARAMETERS_H

#include <gsl/gsl_vector.h>

#include <Eigen/SparseCore>

namespace ohkl {

//! Encapsulates parameters to be passed to ohkl::Minimizer.

class FitParameters {
 public:
    //! Add a parameter to be fit. The original value of the parameter is stored
    //! (see reset()). The return value is the index of the parameter.
    int addParameter(double* addr);
    //! Sets values of the parameters from a GSL vector.
    void setValues(const gsl_vector* v);
    //! Write the values to a GSL vector
    void writeValues(gsl_vector* v) const;
    //! Returns the number of parameters
    size_t nparams() const;
    //! Returns the number of free parameters
    size_t nfree() const;
    //! Sets the constraint matrix
    void setConstraint(const Eigen::SparseMatrix<double>& C);
    //! Remove the constraints
    void resetConstraints();
    //! Kernel of the constraints: this is the transformation from constrained
    //! parameters to original parameters.
    const Eigen::MatrixXd& kernel() const;
    //! Sets the kernel of the constraints.
    void setKernel(const Eigen::MatrixXd& ker);

    //! Reset parameter values to their original state.
    void reset();
    //! Return the parameter vector
    std::vector<double*> values() { return _params; };

 private:
    //! Vector of addresses to fit parameters. Pointers are _not_ owned.
    std::vector<double*> _params;
    std::vector<double> _originalValues;
    //! Linear transformation from kernel to parameters. The columns of K form a
    //! basis for the kernel of _C.
    Eigen::MatrixXd _K;

    //! Projection from unconstrained parameter space to constrained parameter
    //! space.
    Eigen::MatrixXd _P;

    //! Need to update constraints
    bool _constrained;
};

} // namespace ohkl

#endif // NSX_BASE_FIT_FITPARAMETERS_H
