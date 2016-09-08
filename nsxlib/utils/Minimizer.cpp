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

#include "Minimizer.h"


#include <stdlib.h>
#include <stdio.h>


namespace SX {

namespace Utils {





int expb_df (const gsl_vector * x, void *data, gsl_matrix * J)
{
    size_t n = ((struct data *)data)->n;
    double A = gsl_vector_get (x, 0);
    double lambda = gsl_vector_get (x, 1);
    size_t i;
    for (i = 0; i < n; i++)
    {
        /* Jacobian matrix J(i,j) = dfi / dxj, */
        /* where fi = (Yi - yi)/sigma[i], */
        /* Yi = A * exp(-lambda * i) + b */
        /* and the xj are the parameters (A,lambda,b) */
        double t = i;
        double e = exp(-lambda * t);
        gsl_matrix_set (J, i, 0, e);
        gsl_matrix_set (J, i, 1, -t * A * e);
        gsl_matrix_set (J, i, 2, 1.0);
    }
    return GSL_SUCCESS;
}

void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w)
{
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);
    fprintf(stderr, "iter %2zu: A = %.4f, lambda = %.4f, b = %.4f, cond(J) = %8.4f, | %f\n",
            iter,
            gsl_vector_get(x, 0),
            gsl_vector_get(x, 1),
            gsl_vector_get(x, 2),
            1.0 / rcond,
            gsl_blas_dnrm2(f));
}


Minimizer::Minimizer()
{
    _x = nullptr;
    _wt = nullptr;
}

void Minimizer::init(int param, int values)
{
    T = gsl_multifit_nlinear_trust;
    fdf_params = gsl_multifit_nlinear_default_parameters();

    //fdf_params.scale = gsl_multilarge_nlinear_scale_levenberg;
    //fdf_params.trs = gsl_multilarge_nlinear_trs_lm;

    fdf_params.trs = gsl_multifit_nlinear_trs_lm;
//    solve_system(x, &fdf, &fdf_params);
//    fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel;
//    solve_system(x, &fdf, &fdf_params);
//    fdf_params.trs = gsl_multifit_nlinear_trs_dogleg;
//    solve_system(x, &fdf, &fdf_params);
//    fdf_params.trs = gsl_multifit_nlinear_trs_ddogleg;
//    solve_system(x, &fdf, &fdf_params);
//    fdf_params.trs = gsl_multifit_nlinear_trs_subspace2D;
//    solve_system(x, &fdf, &fdf_params);

    _p = param;
    _n = values;

    size_t i;

    /* define the function to be minimized */
    fdf.f = &Minimizer::gsl_f_wrapper;
    //fdf.df = expb_df; /* set to NULL for finite-difference Jacobian */
    fdf.df = nullptr;
    fdf.fvv = NULL;  /* not using geodesic acceleration */

    fdf.p = _p;
    fdf.n = _n;

    fdf.params = this;
    /* this is the data to be fitted */

    /* allocate workspace with default parameters */
    w = gsl_multifit_nlinear_alloc (T, &fdf_params, _n, _p);
}

void Minimizer::fit(int max_iter)
{
    /* initialize solver with starting point and weights */
    gsl_multifit_nlinear_winit (_x, _wt, &fdf, w);
    /* compute initial cost function */
    f = gsl_multifit_nlinear_residual(w);
    gsl_blas_ddot(f, f, &chisq0);
    /* solve the system with a maximum of 20 iterations */
    status = gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol, callback, NULL, &info, w);
    /* compute covariance of best fit parameters */
    J = gsl_multifit_nlinear_jac(w);
    gsl_multifit_nlinear_covar (J, 0.0, covar);
    /* compute final cost */
    gsl_blas_ddot(f, f, &chisq);
#define FIT(i) gsl_vector_get(w->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))
    fprintf(stderr, "summary from method ’%s/%s’\n", gsl_multifit_nlinear_name(w), gsl_multifit_nlinear_trs_name(w));
    fprintf(stderr, "number of iterations: %zu\n", gsl_multifit_nlinear_niter(w));
    fprintf(stderr, "function evaluations: %zu\n", fdf.nevalf);

    fprintf(stderr, "Jacobian evaluations: %zu\n", fdf.nevaldf);
    fprintf(stderr, "reason for stopping: %s\n", (info == 1) ? "small step size" : "small gradient");
    fprintf(stderr, "initial |f(x)| = %f\n", sqrt(chisq0));
    fprintf(stderr, "final |f(x)| = %f\n", sqrt(chisq));
    {
        double dof = _n - _p;
        double c = GSL_MAX_DBL(1, sqrt(chisq / dof));
        fprintf(stderr, "chisq/dof = %g\n", chisq / dof);
        fprintf (stderr, "A = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
        fprintf (stderr, "lambda = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
        fprintf (stderr, "b = %.5f +/- %.5f\n", FIT(2), c*ERR(2));
    }
    fprintf (stderr, "status = %s\n", gsl_strerror (status));

    return;
}

void Minimizer::free()
{
    gsl_multifit_nlinear_free (w);
    gsl_matrix_free (covar);
}

void Minimizer::setInitialValues(const Eigen::VectorXd &x)
{
    assert(_p == x.size());

    if ( _x)
        gsl_vector_free(_x);

    _x = gsl_vector_alloc(x.size());

    for ( int i = 0; i < x.size(); ++i)
        gsl_vector_set(_x, i, x(i));


    covar = gsl_matrix_alloc (_p, _p);
}

void Minimizer::setInitialWeights(const Eigen::VectorXd &wt)
{
    assert(_n == wt.size());

    if ( _wt)
        gsl_vector_free(_wt);

    _wt = gsl_vector_alloc(wt.size());

    for ( int i = 0; i < wt.size(); ++i)
        gsl_vector_set(_wt, i, wt(i));
}

int Minimizer::gsl_f_wrapper(const gsl_vector *input, void *data, gsl_vector *output)
{
    Eigen::VectorXd in(input->size), out(output->size);

    // convert input to Eigen vectors
    for (int i = 0; i < input->size; ++i)
        in(i) = gsl_vector_get(input, i);

    // call the function
    Minimizer* self = reinterpret_cast<Minimizer*>(data);
    int result = self->_f(in, out);

    // convert output to GSL vectors
    for (int i = 0; i < output->size; ++i)
        gsl_vector_set(output, i, out(i));

    return result;
}


} // namespace Utils

} // namespace SX
