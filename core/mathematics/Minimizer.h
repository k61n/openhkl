/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include <functional>

#include <Eigen/Dense>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include "FitParameters.h"


namespace nsx {
struct MinimizerGSL;

//! \class Minimizer
//! \brief Class to wrap the GSL non-linear least squares minimization routines.
//! This class supports both numerical and analytic derivatives. 
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

    //! Copy constructor is deleted, to avoid possibiliy of copying the GSL workspace.
    Minimizer(const Minimizer& other) = delete;
    //! Assignmenrt operator is deleted, to avoid possibility of copying the GSL workspace.
    Minimizer& operator=(const Minimizer& other) = delete;

    //! Initialize a GSL workspace with given number of parameters and values.
    //! \param [in] params The number of parameters to fit.
    //! \param [in] values The size of the residual vector.
    void initialize(FitParameters& params, int values);

    //! Return the GSL status string
    const char* getStatusStr();
    //! Run the fitting routine with the specified maximum number of iterations.
    bool fit(int max_iter);
    //! Return the variance-covariance matrix of the fit parameters.
    Eigen::MatrixXd covariance();
    //! Set the relative tolerance in the fit parameters.
    void setxTol(double xtol);
    //! Set the relative tolerance in the gradient (Jacobian).
    void setgTol(double gtol);
    //! Set the relative tolerance in the residuals.
    void setfTol(double ftol);
    //! Return the Jacobian matrix.
    Eigen::MatrixXd jacobian();
    //! Set the weights of the residuals.
    void setWeights(const Eigen::VectorXd& wt);
    //! Set the function which computes the least-squares residuals.
    template <typename Fun_>
    void set_f(Fun_ functor)
    {
        _f = static_cast<f_type>(functor);
    }
    //! Set the function computes the Jacobian of the residuals.
    //! The default value is \p nullptr, in which case a numerical derivative is used instead.
    template <typename Fun_>
    void set_df(Fun_ functor)
    {
        _df = static_cast<df_type>(functor);
    }
    //! Return the mean squared error with respect to the current minimizer values.
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

} // end namespace nsx
