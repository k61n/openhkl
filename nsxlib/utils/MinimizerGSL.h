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

#ifndef NSXTOOL_MINIMIZERGSL_H_
#define NSXTOOL_MINIMIZERGSL_H_

#include "IMinimizer.h"

#include <functional>
#include <Eigen/Dense>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#if ((NSXTOOL_GSL_VERSION_MAJOR == 2) && (NSXTOOL_GSL_VERSION_MINOR >= 2) )
#include <gsl/gsl_multifit_nlinear.h>
#else
#include <gsl/gsl_multifit_nlin.h>
#endif

namespace SX {

namespace Utils {

class MinimizerGSL: public IMinimizer {
public:
    MinimizerGSL();
    ~MinimizerGSL();

    void initialize(int params, int values) override;
    void deinitialize() override;

    const char* getStatusStr() override;
    bool fit(int max_iter) override;

private:
    static int gsl_f_wrapper(const gsl_vector*, void*, gsl_vector*);

    static void eigenFromGSL(const gsl_vector* in, Eigen::VectorXd& out);
    static void eigenFromGSL(const gsl_matrix* in, Eigen::MatrixXd& out);

    static void gslFromEigen(const Eigen::VectorXd& in, gsl_vector* out);
    static void gslFromEigen(const Eigen::MatrixXd& in, gsl_matrix* out);

#if ((NSXTOOL_GSL_VERSION_MAJOR == 2) && (NSXTOOL_GSL_VERSION_MINOR >= 2) )
    gsl_multifit_nlinear_workspace* _workspace;
    gsl_multifit_nlinear_fdf _fdf;
    gsl_multifit_nlinear_parameters _fdfParams;
#else
    gsl_multifit_fdfsolver* _workspace;
    gsl_multifit_function_fdf _fdf;
#endif

    // gsl_vector *f;
    gsl_matrix *_jacobian_gsl;
    gsl_matrix* _covariance_gsl;

    int _status, info;

    gsl_vector* _x_gsl;
    gsl_vector* _wt_gsl;

    Eigen::VectorXd _inputEigen;
    Eigen::VectorXd _outputEigen;
};

} // end namespace Utils

} // end namespace SX

#endif // NSXTOOL_MINIMIZERGSL_H_
