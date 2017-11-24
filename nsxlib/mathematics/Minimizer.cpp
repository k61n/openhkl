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

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

#include <Eigen/Dense>

#include "Minimizer.h"

#include <iostream>

namespace nsx {

void callback_helper(const size_t iter, void* data, const gsl_multifit_nlinear_workspace *w)
{
    Minimizer* self = reinterpret_cast<Minimizer*>(data);    
    double r = 0.0;
    for (auto i = 0; i < w->f->size; ++i) {
        r += gsl_vector_get(w->f, i) * gsl_vector_get(w->f, i);
    }
    std::cout << "iteration " << iter << "; " << r << std::endl;
}

// Helper class to wrap GSL data structures. Used only in implementation of Minimizer.
struct MinimizerGSL {
    gsl_multifit_nlinear_workspace* workspace;
    gsl_multifit_nlinear_fdf fdf;
    gsl_multifit_nlinear_parameters fdfParams;
    gsl_matrix *jacobian;
    gsl_matrix* covariance;
    int status, info;
    gsl_vector* x;
    gsl_vector* wt;
    
    MinimizerGSL(): workspace(nullptr), jacobian(nullptr), covariance(nullptr), x(nullptr), wt(nullptr) {};
};

/*
static int gsl_f_wrapper(const gsl_vector*, void*, gsl_vector*);
static int gsl_df_wrapper(const gsl_vector*, void*, gsl_matrix*);
*/
static void eigenFromGSL(const gsl_vector* in, Eigen::VectorXd& out);
static void eigenFromGSL(const gsl_matrix* in, Eigen::MatrixXd& out);
static void gslFromEigen(const Eigen::VectorXd& in, gsl_vector* out);
static void gslFromEigen(const Eigen::MatrixXd& in, gsl_matrix* out);

Minimizer::Minimizer():
    _gsl(new MinimizerGSL),
    _numValues(0),
    _xtol(1e-7),
    _gtol(1e-7),
    _ftol(1e-7), 
    _f(nullptr), 
    _df(nullptr)

{
}

Minimizer::~Minimizer()
{
    cleanup();
    delete _gsl;
}

void Minimizer::initialize(FitParameters& params, int values)
{
    cleanup();
    _params = params;
    _numValues = values;
    _wt.resize(_numValues);
    _outputEigen.resize(_numValues);

    const auto nfree = _params.nfree();
    const auto nparams = _params.nparams();

    for (int i = 0; i < _numValues; ++i) {
        _wt(i) = 1.0;
    }

    _jacobian.resize(_numValues, nfree);
    _covariance.resize(nfree, nfree);
    _gsl->fdfParams = gsl_multifit_nlinear_default_parameters();

    // allocate initial paramter values and weights
    _gsl->x = gsl_vector_alloc(nfree);
    _gsl->wt = gsl_vector_alloc(_numValues);
    _gsl->covariance = gsl_matrix_alloc(nfree, nfree);

    // initialize the weights to 1
    for (int i = 0; i < _numValues; ++i) {
        gsl_vector_set(_gsl->wt, i, 1.0);
    }
    
    _gsl->workspace = gsl_multifit_nlinear_alloc(gsl_multifit_nlinear_trust, &_gsl->fdfParams, _numValues, nfree);
}

bool Minimizer::fit(int max_iter)
{
    if (!_gsl->workspace || !_f) {
        return false;
    }

    const auto nfree = _params.nfree();

    // too few data points to fit
    if (_numValues < nfree) {
        return false;
    }

    // function which computes vector of residuals
    _gsl->fdf.f = &Minimizer::gsl_f_wrapper; 
    _gsl->fdf.df = _df ? &Minimizer::gsl_df_wrapper : nullptr;
    _gsl->fdf.p = nfree; // number of parameters to be fit
    _gsl->fdf.n = _numValues; // number of residuals
    _gsl->fdf.params = this; // this is the data ptr which is passed to gsl_f_wrapper
    _gsl->fdf.fvv = nullptr;  // not using geodesic acceleration
    
    if (_df) {
        _dfInputEigen.resize(nfree);
        _dfOutputEigen.resize(_numValues, nfree);
    }

    // initialize solver with starting point and weights
    // note that this is NOT called in MinimizerGSL::initialize because _f needs to be set first!
    _params.writeValues(_gsl->x);
    gslFromEigen(_wt, _gsl->wt);

    gsl_multifit_nlinear_winit(_gsl->x, _gsl->wt, &_gsl->fdf, _gsl->workspace);
    _gsl->status = gsl_multifit_nlinear_driver(max_iter, _xtol, _gtol, _ftol, &callback_helper, this, &_gsl->info, _gsl->workspace);
    gsl_multifit_nlinear_covar(_gsl->workspace->J, 1e-6, _gsl->covariance);
    _numIter = _gsl->workspace->niter;

    eigenFromGSL(_gsl->workspace->J, _jacobian);
    _params.setValues(_gsl->workspace->x);
    eigenFromGSL(_gsl->covariance, _covariance);

    return (_gsl->status == GSL_SUCCESS);
}

void Minimizer::cleanup()
{
    if (_gsl->workspace) {
        gsl_multifit_nlinear_free(_gsl->workspace);
        _gsl->workspace = nullptr;
    }

    if (_gsl->covariance) {
        gsl_matrix_free(_gsl->covariance);
        _gsl->covariance = nullptr;
    }

    if (_gsl->jacobian) {
        // jacobian is part of the workspace, doesnt need to be freed
        gsl_matrix_free(_gsl->jacobian);
        _gsl->jacobian = nullptr;
    }

    if (_gsl->x) {
        gsl_vector_free(_gsl->x);
        _gsl->x = nullptr;
    }

    if (_gsl->wt) {
        gsl_vector_free(_gsl->wt);
        _gsl->wt = nullptr;
    }

    _f = nullptr;
    _df = nullptr;
}


const char *Minimizer::getStatusStr()
{
    return gsl_strerror(_gsl->status);
}


int Minimizer::gsl_f_wrapper(const gsl_vector *input, void *data, gsl_vector *output)
{
    // call the function
    Minimizer* self = reinterpret_cast<Minimizer*>(data);
    assert(self->_f != nullptr);

    // update parameters
    self->_params.setValues(input);

    // compute value
    int result = self->_f(self->_outputEigen);

    // convert output to GSL vectors
    gslFromEigen(self->_outputEigen, output);

    return result;
}

int Minimizer::gsl_df_wrapper(const gsl_vector* input, void* data, gsl_matrix* output)
{
    // call the function
    Minimizer* self = reinterpret_cast<Minimizer*>(data);
    assert(self->_df != nullptr);   

    // update the parameters
    self->_params.setValues(input);
    // calculate jacobian   
    int result = self->_df(self->_dfOutputEigen);
    // convert output to GSL vectors
    gslFromEigen(self->_dfOutputEigen, output);

    return result;
}

void eigenFromGSL(const gsl_vector *in, Eigen::VectorXd &out)
{
    assert(int(in->size) == out.size());

    for (unsigned i = 0; i < in->size; ++i)
        out(i) = gsl_vector_get(in, i);
}

void eigenFromGSL(const gsl_matrix *in, Eigen::MatrixXd &out)
{
    assert(int(in->size1) == out.rows());
    assert(int(in->size2) == out.cols());

    for (unsigned int i = 0; i < in->size1; ++i)
        for (unsigned j = 0; j < in->size2; ++j)
            out(i, j) = gsl_matrix_get(in, i, j);
}

static void gslFromEigen(const Eigen::VectorXd &in, gsl_vector *out)
{
    assert(in.size() == int(out->size));

    for (unsigned int i = 0; i < out->size; ++i)
        gsl_vector_set(out, i, in(i));
}

void gslFromEigen(const Eigen::MatrixXd &in, gsl_matrix *out)
{
    assert(int(out->size1) == in.rows());
    assert(int(out->size2) == in.cols());


    for (unsigned int i = 0; i < out->size1; ++i)
        for (unsigned int j = 0; j < out->size2; ++j)
            gsl_matrix_set(out, i, j, in(i, j));
}

Eigen::MatrixXd Minimizer::covariance()
{
    const auto& K = _params.kernel();
    return K*_covariance*K.transpose();
}

void Minimizer::setxTol(double xtol)
{
    _xtol = xtol;
}

void Minimizer::setgTol(double gtol)
{
    _gtol = gtol;
}

void Minimizer::setfTol(double ftol)
{
    _ftol = ftol;
}

Eigen::MatrixXd Minimizer::jacobian()
{
    const auto& K = _params.kernel();
    return _jacobian*K.transpose();
}

void Minimizer::setWeights(const Eigen::VectorXd &wt)
{
    assert(_numValues == wt.size());
    _wt = wt;
}

int Minimizer::numIterations()
{
    return _numIter;
}

double Minimizer::meanSquaredError() const
{
    const gsl_vector* residual = _gsl->workspace->f;
    const int size = residual->size;
    double mse = 0.0;

    for (int i = 0; i < size; ++i) {
        const double ri = gsl_vector_get(residual, i);
        mse += ri*ri;
    }
    return mse / (size - _params.nfree());
}

} // end namespace nsx
