//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/fit/MinimizerNLopt.h
//! @brief     Defines class MinimizerNLopt
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_FIT_MINIMIZERNLOPT_H
#define OHKL_BASE_FIT_MINIMIZERNLOPT_H

#include <nlopt.hpp>

#include <optional>

namespace ohkl {

struct NLoptFitData {
    NLoptFitData(unsigned int size);

    std::size_t npoints;
    std::vector<double> x_data;
    std::vector<double> y_data;
};

struct EqualityConstraintData {
    // Fix a parameter: params[index] = value
    int index;
    double value;
};

struct InequalityConstraintData {
    // a * x_{n} <= b * x_{n-1}
    // a * x_{n} - b * x_{n-1} <= 0
    int n;
    double a;
    double b;
};

//! Wraps the NLopt non-linear least squares minimization routines.
class MinimizerNLopt {
 public:
    MinimizerNLopt(unsigned int nparams, nlopt::vfunc objective, void* f_data);

    void addEqualityConstraint(nlopt::vfunc constraint, void* c_data);
    void addInequalityConstraint(nlopt::vfunc constraint, void* c_data);
    //! Set the optimization tolerance
    void setFTol(double ftol) { _ftol = ftol; };
    //! Set the parameter tolerance
    void setXTol(double xtol) { _xtol = xtol; };
    //! Set the constraint tolerance
    void setCTol(double ctol) { _ctol = ctol; };
    //! Set initial step size
    void setInitStep(double init_step) { _init_step = init_step; };
    //! Set maximum number of iterations
    void setMaxIter(double max_iter) { _max_iter = max_iter; };
    //! Perform the minimization
    std::optional<double> minimize(std::vector<double>& parameters);

 private:
    //! Optimisation algorithm to use
    nlopt::algorithm _algo;
    //! The NLopt optimzer opbject
    nlopt::opt _optimizer;
    //! Relative tolerance on sum of squared residuals
    double _ftol;
    //! Relative tolerance on parameters
    double _xtol;
    //! Constraint tolerance
    double _ctol;
    //! Initial step size
    double _init_step;
    //! Maximum number of iterations
    int _max_iter;
};

} // namespace ohkl

#endif // OHKL_BASE_FIT_MINIMIZERNLOPT_H
