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

 Forshungszentrum Juelich GmbH
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

#ifndef NSXTOOL_MINIMIZER_H_
#define NSXTOOL_MINIMIZER_H_

#include <functional>
#include <Eigen/Dense>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_multilarge_nlinear.h>

namespace SX {

namespace Utils {


struct data {
    size_t n;
    double * y;
};

class Minimizer {
public:
    using f_type = std::function<int(const Eigen::VectorXd&, Eigen::VectorXd&)>;

    Minimizer();
    ~Minimizer();

    void initialize(int params, int values);
    bool fit(int max_iter);
    void deinitialize();

    template <typename Fun_>
    void set_f(Fun_ functor)
    {
        _f = static_cast<f_type>(functor);
    }

    void setInitialValues(const Eigen::VectorXd& x);

    void setInitialWeights(const Eigen::VectorXd& wt);

    Eigen::MatrixXd covariance();
    Eigen::MatrixXd jacobian();
    Eigen::VectorXd params();

    const char* getStatusStr();

    void setxTol(double xtol);
    void setgTol(double gtol);
    void setfTol(double ftol);

    int numIterations();

private:
    static int gsl_f_wrapper(const gsl_vector*, void*, gsl_vector*);

    static void eigenFromGSL(const gsl_vector* in, Eigen::VectorXd& out);
    static void eigenFromGSL(const gsl_matrix* in, Eigen::MatrixXd& out);

    static void gslFromEigen(const Eigen::VectorXd& in, gsl_vector* out);
    static void gslFromEigen(const Eigen::MatrixXd& in, gsl_matrix* out);

    gsl_multifit_nlinear_workspace* _workspace;

    gsl_matrix* _covariance;
    gsl_multifit_nlinear_fdf _fdf;
    gsl_multifit_nlinear_parameters _fdfParams;

    int _numValues, _numParams;

    gsl_vector *f;
    gsl_matrix *_jacobian;
    double _xtol = 1e-7;
    double _gtol = 1e-7;
    double _ftol = 0.0;
    //double chisq, chisq0;
    int _status, info;
    struct data d;

    gsl_vector* _x;
    gsl_vector* _wt;

    Eigen::VectorXd _inputEigen;
    Eigen::VectorXd _outputEigen;

    f_type _f;


};

} // namespace Utils

} // namespace SX

#endif // NSXTOOL_MINIMIZER_H_
