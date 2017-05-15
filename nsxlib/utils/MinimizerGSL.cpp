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

#include <stdlib.h>
#include <stdio.h>

#include "MinimizerGSL.h"

namespace nsx {

namespace Utils {

MinimizerGSL::MinimizerGSL():
    IMinimizer(),
    _workspace(nullptr),
    _jacobian_gsl( nullptr),
    _covariance_gsl(nullptr),
    _x_gsl (nullptr),
    _wt_gsl (nullptr)
{
}

MinimizerGSL::~MinimizerGSL()
{
    deinitialize();
}

void MinimizerGSL::initialize(int params, int values)
{
    deinitialize();
    IMinimizer::initialize(params, values);

    // allocate initial paramter values and weights
    _x_gsl = gsl_vector_alloc(_numParams);
    _wt_gsl = gsl_vector_alloc(_numValues);
    _covariance_gsl = gsl_matrix_alloc(_numParams, _numParams);

    // initialize the weights to 1
    for (int i = 0; i < _numValues; ++i)
        gsl_vector_set(_wt_gsl, i, 1.0);

    // these are used in the helper routine gsl_f_wrapper
    _inputEigen.resize(_numParams);
    _outputEigen.resize(_numValues);

    _fdf.f = &MinimizerGSL::gsl_f_wrapper; // function to be minimized: wrapper which calls _f
    _fdf.df = nullptr;  // set to nullptr since we compute the jacobian numerically

    _fdf.p = _numParams; // number of parameters to be fit
    _fdf.n = _numValues; // number of residuals
    _fdf.params = this; // this is the data ptr which is passed to gsl_f_wrapper

    // use default parameters + Levenberg-Marquardt trust region selection
#if ((NSXTOOL_GSL_VERSION_MAJOR == 2) && (NSXTOOL_GSL_VERSION_MINOR >= 2) )
    _fdfParams = gsl_multifit_nlinear_default_parameters();
    _fdfParams.trs = gsl_multifit_nlinear_trs_lm;
    _fdf.fvv = nullptr;  // not using geodesic acceleration
     _workspace = gsl_multifit_nlinear_alloc(gsl_multifit_nlinear_trust, &_fdfParams, _numValues, _numParams);
#else
    _workspace = gsl_multifit_fdfsolver_alloc(gsl_multifit_fdfsolver_lmsder, _numValues, _numParams);
#endif
}

bool MinimizerGSL::fit(int max_iter)
{
    if (!_workspace || !_f)
        return false;

    // initialize solver with starting point and weights
    // note that this is NOT called in MinimizerGSL::initialize because _f needs to be set first!
    gslFromEigen(_x, _x_gsl);
    gslFromEigen(_wt, _wt_gsl);

#if ((NSXTOOL_GSL_VERSION_MAJOR == 2) && (NSXTOOL_GSL_VERSION_MINOR >= 2) )
    gsl_multifit_nlinear_winit (_x_gsl, _wt_gsl, &_fdf, _workspace);
    _status = gsl_multifit_nlinear_driver(max_iter, _xtol, _gtol, _ftol, NULL /*callback*/, NULL, &info, _workspace);
    gsl_multifit_nlinear_covar(_workspace->J, 1e-6, _covariance_gsl);
    _numIter = _workspace->niter;
    eigenFromGSL(_workspace->J, _jacobian);
#else
    gsl_multifit_fdfsolver_set(_workspace, &_fdf, _x_gsl);
    _jacobian_gsl = gsl_matrix_alloc(_numValues, _numParams);
    _status = gsl_multifit_fdfsolver_driver(_workspace, max_iter, _xtol, _gtol, _ftol, &info);
    gsl_multifit_fdfsolver_dif_df(_workspace->x, _wt_gsl, &_fdf, _workspace->f, _jacobian_gsl);
    eigenFromGSL(_jacobian_gsl, _jacobian);
    gsl_matrix_free(_jacobian_gsl);
    _jacobian_gsl = nullptr;
    _numIter = _workspace->niter;
#endif

    //fprintf (stderr, "status = %s\n", gsl_strerror(_status));

    eigenFromGSL(_workspace->x, _x);

    gslFromEigen(_x, _x_gsl);

    eigenFromGSL(_covariance_gsl, _covariance);

    return (_status == GSL_SUCCESS);
}

void MinimizerGSL::deinitialize()
{
    IMinimizer::deinitialize();

    if (_workspace) {
#if ((NSXTOOL_GSL_VERSION_MAJOR == 2) && (NSXTOOL_GSL_VERSION_MINOR >= 2) )
        gsl_multifit_nlinear_free (_workspace);
#else
        gsl_multifit_fdfsolver_free(_workspace);
#endif
        _workspace = nullptr;
    }

    if (_covariance_gsl) {
        gsl_matrix_free (_covariance_gsl);
        _covariance_gsl = nullptr;
    }

    if (_jacobian_gsl) {
        // jacobian is part of the workspace, doesnt need to be freed
        //gsl_matrix_free(_jacobian);
        _jacobian_gsl = nullptr;
    }

    if (_x_gsl) {
        gsl_vector_free(_x_gsl);
        _x_gsl = nullptr;
    }

    if (_wt_gsl) {
        gsl_vector_free(_wt_gsl);
        _wt_gsl = nullptr;
    }

    _f = nullptr;
}


const char *MinimizerGSL::getStatusStr()
{
    return gsl_strerror(_status);
}


int MinimizerGSL::gsl_f_wrapper(const gsl_vector *input, void *data, gsl_vector *output)
{
    // call the function
    MinimizerGSL* self = reinterpret_cast<MinimizerGSL*>(data);

    Eigen::VectorXd& in = self->_inputEigen;
    Eigen::VectorXd& out = self->_outputEigen;

    // convert input to Eigen vectors
    eigenFromGSL(input, in);

    int result = self->_f(in, out);

    // convert output to GSL vectors
    gslFromEigen(out, output);

    return result;
}

void MinimizerGSL::eigenFromGSL(const gsl_vector *in, Eigen::VectorXd &out)
{
    assert(in->size == out.size());

    for (int i = 0; i < in->size; ++i)
        out(i) = gsl_vector_get(in, i);
}

void MinimizerGSL::eigenFromGSL(const gsl_matrix *in, Eigen::MatrixXd &out)
{
    assert(in->size1 == out.rows());
    assert(in->size2 == out.cols());

    for (int i = 0; i < in->size1; ++i)
        for (int j = 0; j < in->size2; ++j)
            out(i, j) = gsl_matrix_get(in, i, j);
}

void MinimizerGSL::gslFromEigen(const Eigen::VectorXd &in, gsl_vector *out)
{
    assert(in.size() == out->size);

    for (int i = 0; i < out->size; ++i)
        gsl_vector_set(out, i, in(i));
}

void MinimizerGSL::gslFromEigen(const Eigen::MatrixXd &in, gsl_matrix *out)
{
    assert(out->size1 == in.rows());
    assert(out->size2 == in.cols());


    for (int i = 0; i < out->size1; ++i)
        for (int j = 0; j < out->size2; ++j)
            gsl_matrix_set(out, i, j, in(i));
}


} // end namespace Utils

} // end namespace nsx
