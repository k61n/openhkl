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

//! Wraps the NLopt non-linear least squares minimization routines.
class MinimizerNLopt {
 public:
    MinimizerNLopt();

    //! Add a parameter of the optimisation problem
    void addParameter(double* address);
    //! Transfer parameters from pointers to local vector
    std::vector<double> grabParameters();
    //! Transfer parameters from local vector to pointers
    void setParameters(const std::vector<double> params);
    //! Initialise optimizer, set objective and constraint functions
    void init(void* f_data, nlopt::vfunc objective, std::optional<nlopt::vfunc> constraint = {});
    //! Set the optimization tolerance
    void setFTol(double ftol) { _ftol = ftol; };
    //! Set the constraint tolerance
    void setCTol(double ctol) { _ctol = ctol; };
    //! Perform the minimization
    std::optional<double> minimize();

 private:
    //! Optimisation algorithm to use
    nlopt::algorithm _algo;
    //! Vector of pointers to parameters
    std::vector<double*> _parameters;
    //! The NLopt optimzer opbject
    nlopt::opt _optimizer;
    //! Relative tolerance on sum of squared residuals
    double _ftol;
    //! Constraint tolerance
    double _ctol;
    //! Maximum number of iterations
    int _max_iter;
};

} // namespace ohkl

#endif // OHKL_BASE_FIT_MINIMIZERNLOPT_H
