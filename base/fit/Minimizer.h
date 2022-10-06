//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/fit/Minimizer.h
//! @brief     Defines class Minimizer
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_FIT_MINIMIZER_H
#define OHKL_BASE_FIT_MINIMIZER_H

#include "base/fit/FitParameters.h"

#include <gsl/gsl_matrix.h>

namespace ohkl {

struct MinimizerGSL;

//! Wraps the GSL non-linear least squares minimization routines.

//! Supports both numerical and analytic derivatives.

class Minimizer {
 public:
    //! The signature of the function f which specifies the vector of residuals.
    using f_type = std::function<int(Eigen::VectorXd&)>;
    //! The signature of the function df which is the Jacobian of the residuals.
    using df_type = std::function<int(Eigen::MatrixXd&)>;

    //! Default construct: initializes parameters to reasonable default values.
    Minimizer();
    //! Destructor: ensures that GSL internals are cleaned up.
    ~Minimizer();

    //! Copy constructor is deleted, to avoid possibiliy of copying the GSL
    //! workspace.
    Minimizer(const Minimizer& other) = delete;
    //! Assignmenrt operator is deleted, to avoid possibility of copying the GSL
    //! workspace.
    Minimizer& operator=(const Minimizer& other) = delete;

    //! Initialize a GSL workspace with given number of parameters and values.
    //! @param [in] params The number of parameters to fit.
    //! @param [in] values The size of the residual vector.
    void initialize(FitParameters& params, int values);

    //! Returns the GSL status string
    const char* getStatusStr();
    //! Run the fitting routine with the specified maximum number of iterations.
    bool fit(int max_iter);
    //! Returns the variance-covariance matrix of the fit parameters.
    Eigen::MatrixXd covariance();
    //! Sets the relative tolerance in the fit parameters.
    void setxTol(double xtol);
    //! Sets the relative tolerance in the gradient (Jacobian).
    void setgTol(double gtol);
    //! Sets the relative tolerance in the residuals.
    void setfTol(double ftol);
    //! Returns the Jacobian matrix.
    Eigen::MatrixXd jacobian();
    //! Sets the weights of the residuals.
    void setWeights(const Eigen::VectorXd& wt);
    //! Sets the function which computes the least-squares residuals.
    template <typename Fun_> void set_f(Fun_ functor) { _f = static_cast<f_type>(functor); }
    //! Sets the function computes the Jacobian of the residuals.
    //! The default value is \p nullptr, in which case a numerical derivative is used instead.
    template <typename Fun_> void set_df(Fun_ functor) { _df = static_cast<df_type>(functor); }
    //! Returns the mean squared error with respect to the current minimizer values.
    double meanSquaredError() const;

 private:
    //! Clean up the GSL workspace and allocated vectors.
    void cleanup();
    //! Private implementation details of the GSL wrapper
    MinimizerGSL* _gsl;
    //! GSL wrapper function. Static method because it needs access to private members
    static int gsl_f_wrapper(const gsl_vector*, void*, gsl_vector*);
    //! GSL wrapper function. Static method because it needs access to private members
    static int gsl_df_wrapper(const gsl_vector*, void*, gsl_matrix*);
    //! Implementation detail: used to convert between Eigen and GSL vectors.
    Eigen::VectorXd _dfInputEigen;
    //! Implementation detail: used to convert between Eigen and GSL vectors.
    Eigen::VectorXd _outputEigen;
    //! Implementation detail: used to convert between Eigen and GSL vectors.
    Eigen::MatrixXd _dfOutputEigen;
    //! Vector of weights for the residuals.
    Eigen::VectorXd _wt;
    //! Jacobian matrix of the residual function.
    Eigen::MatrixXd _jacobian;
    //! Variance-covariance matrix of the fit parameters.
    Eigen::MatrixXd _covariance;
    //! Number of values in the fit, i.e. size of residual vector.
    size_t _numValues;
    //! Relative tolerance of parameters.
    double _xtol;
    //! Relative tolerance of gradient.
    double _gtol;
    //! Relative tolerance of residuals.
    double _ftol;
    //! The function computing the residuals.
    f_type _f;
    //! The function computing the analytic gradient.
    df_type _df;
    //! Pointers to the fit parameters
    FitParameters _params;
};

} // namespace ohkl

#endif // OHKL_BASE_FIT_MINIMIZER_H
