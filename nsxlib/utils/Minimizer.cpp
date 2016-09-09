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


Minimizer::Minimizer()
{


    _workspace = nullptr;

    _covariance = nullptr;
    _numValues = 0;
    _numParams = 0;

    f = nullptr;
    _jacobian = nullptr;

    _x = nullptr;
    _wt = nullptr;

    f_type _f = nullptr;
}

Minimizer::~Minimizer()
{
    deinitialize();
}

void Minimizer::initialize(int params, int values)
{
    deinitialize();

    // use default parameters + Levenberg-Marquardt trust region selection
    _fdfParams = gsl_multifit_nlinear_default_parameters();
    _fdfParams.trs = gsl_multifit_nlinear_trs_lm;

    _numParams = params;
    _numValues = values;

    // allocate initial paramter values and weights
    _x = gsl_vector_alloc(_numParams);
    _wt = gsl_vector_alloc(_numValues);

    // these are used in the helper routine gsl_f_wrapper
    _inputEigen.resize(_numParams);
    _outputEigen.resize(_numValues);

    _fdf.f = &Minimizer::gsl_f_wrapper; // function to be minimized: wrapper which calls _f
    _fdf.df = nullptr;  // set to nullptr since we compute the jacobian numerically
    _fdf.fvv = nullptr;  // not using geodesic acceleration

    _fdf.p = _numParams; // number of parameters to be fit
    _fdf.n = _numValues; // number of residuals

    _fdf.params = this; // this is the data ptr which is passed to gsl_f_wrapper

    // allocate workspace with specified parameters
    _workspace = gsl_multifit_nlinear_alloc(gsl_multifit_nlinear_trust, &_fdfParams, _numValues, _numParams);
}

bool Minimizer::fit(int max_iter)
{
    if (!_workspace || !_f)
        return false;

    // initialize solver with starting point and weights
    // note that this is NOT called in Minimizer::initialize because _f needs to be set first!
    gsl_multifit_nlinear_winit (_x, _wt, &_fdf, _workspace);

    // run fitting routine
    status = gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol, NULL /*callback*/, NULL, &info, _workspace);


    fprintf (stderr, "status = %s\n", gsl_strerror (status));

    return status == GSL_SUCCESS;
}

void Minimizer::deinitialize()
{
    if (_workspace) {
        gsl_multifit_nlinear_free (_workspace);
        _workspace = nullptr;
    }

    if (_covariance) {
        gsl_matrix_free (_covariance);
        _covariance = nullptr;
    }

    if (_jacobian) {
        // jacobian is part of the workspace, doesnt need to be freed
        //gsl_matrix_free(_jacobian);
        _jacobian = nullptr;
    }

    if (_x) {
        gsl_vector_free(_x);
        _x = nullptr;
    }

    if (_wt) {
        gsl_vector_free(_wt);
        _wt = nullptr;
    }

    _f = nullptr;
}

void Minimizer::setInitialValues(const Eigen::VectorXd &x)
{
    assert(_numParams == x.size());

    for ( int i = 0; i < x.size(); ++i)
        gsl_vector_set(_x, i, x(i));

    _covariance = gsl_matrix_alloc(_numParams, _numParams);
}

void Minimizer::setInitialWeights(const Eigen::VectorXd &wt)
{
    assert(_numValues == wt.size());

    _wt = gsl_vector_alloc(wt.size());

    for ( int i = 0; i < wt.size(); ++i)
        gsl_vector_set(_wt, i, wt(i));
}

Eigen::MatrixXd Minimizer::covariance()
{
    if (!_jacobian)
        _jacobian = gsl_multifit_nlinear_jac(_workspace);

    gsl_multifit_nlinear_covar(_jacobian, 0.0, _covariance);
    Eigen::MatrixXd c(_numParams, _numParams);
    eigenFromGSL(_covariance, c);
    return c;
}

Eigen::MatrixXd Minimizer::jacobian()
{
    _jacobian = gsl_multifit_nlinear_jac(_workspace);
    Eigen::MatrixXd jac(_jacobian->size1, _jacobian->size2);
    eigenFromGSL(_jacobian, jac);
    return jac;
}

Eigen::VectorXd Minimizer::params()
{
    Eigen::VectorXd x(_numParams);

    if (_workspace)
        eigenFromGSL(_workspace->x, x);

    return x;
}

int Minimizer::gsl_f_wrapper(const gsl_vector *input, void *data, gsl_vector *output)
{
    // call the function
    Minimizer* self = reinterpret_cast<Minimizer*>(data);

    Eigen::VectorXd& in = self->_inputEigen;
    Eigen::VectorXd& out = self->_outputEigen;

    // convert input to Eigen vectors
    eigenFromGSL(input, in);

    int result = self->_f(in, out);

    // convert output to GSL vectors
    gslFromEigen(out, output);

    return result;
}

void Minimizer::eigenFromGSL(const gsl_vector *in, Eigen::VectorXd &out)
{
    assert(in->size == out.size());

    for (int i = 0; i < in->size; ++i)
        out(i) = gsl_vector_get(in, i);
}

void Minimizer::eigenFromGSL(const gsl_matrix *in, Eigen::MatrixXd &out)
{
    assert(in->size1 == out.rows());
    assert(in->size2 == out.cols());

    for (int i = 0; i < in->size1; ++i)
        for (int j = 0; j < in->size2; ++j)
            out(i, j) = gsl_matrix_get(in, i, j);
}

void Minimizer::gslFromEigen(const Eigen::VectorXd &in, gsl_vector *out)
{
    assert(in.size() == out->size);

    for (int i = 0; i < out->size; ++i)
        gsl_vector_set(out, i, in(i));
}

void Minimizer::gslFromEigen(const Eigen::MatrixXd &in, gsl_matrix *out)
{
    assert(out->size1 == in.rows());
    assert(out->size2 == in.cols());


    for (int i = 0; i < out->size1; ++i)
        for (int j = 0; j < out->size2; ++j)
            gsl_matrix_set(out, i, j, in(i));
}


} // namespace Utils

} // namespace SX
