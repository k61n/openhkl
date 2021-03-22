//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/fit/Minimizer.cpp
//! @brief     Implements class Minimizer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cstdio>
#include <cstdlib>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_vector.h>

#include <Eigen/Dense>

#include "base/fit/Minimizer.h"

namespace nsx {

static void error_handler(const char* reason, const char* file, int line, int gsl_errno)
{
    std::string msg = file;
    msg += "(" + std::to_string(line) + "): ";
    msg += reason;
    msg += " (" + std::to_string(gsl_errno) + ")";
    throw std::runtime_error(msg);
}

void callback_helper(const size_t /*iter*/, void* /*data*/, const gsl_multifit_nlinear_workspace* w)
{
    double r = 0.0;
    for (size_t i = 0; i < w->f->size; ++i)
        r += gsl_vector_get(w->f, i) * gsl_vector_get(w->f, i);
}

//! Helper class to wrap GSL data structures. Used only in implementation of
//! Minimizer.
struct MinimizerGSL {
    //! GSL fit workspace
    gsl_multifit_nlinear_workspace* workspace;
    //! GSL Residual/Jacobian structure
    gsl_multifit_nlinear_fdf fdf;
    //! Fit parameters
    gsl_multifit_nlinear_parameters fdfParams;
    //! Workspace for jacobian matrix
    gsl_matrix* jacobian;
    //! Workspace of covariance matrix
    gsl_matrix* covariance;
    //! Status
    int status;
    //! Additional information
    int info;
    //! Vector of fit parameters
    gsl_vector* x;
    //! Weight vector
    gsl_vector* wt;

    MinimizerGSL()
        : workspace(nullptr), jacobian(nullptr), covariance(nullptr), x(nullptr), wt(nullptr){};
};

//! Convert a GSL matrix to an Eigen matrix
static void eigenFromGSL(const gsl_matrix* in, Eigen::MatrixXd& out);
//! Convert and Eigen vector to a GSL vector
static void gslFromEigen(const Eigen::VectorXd& in, gsl_vector* out);
//! Convert an Eigen matrix to a GSL matrix
static void gslFromEigen(const Eigen::MatrixXd& in, gsl_matrix* out);

Minimizer::Minimizer()
    : _gsl(new MinimizerGSL)
    , _numValues(0)
    , _xtol(1e-7)
    , _gtol(1e-7)
    , _ftol(1e-7)
    , _f(nullptr)
    , _df(nullptr)

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
    gsl_set_error_handler(&error_handler);
    _params = params;
    _numValues = values;
    _wt.resize(_numValues);
    _outputEigen.resize(_numValues);

    const auto nfree = _params.nfree();

    for (size_t i = 0; i < _numValues; ++i)
        _wt(i) = 1.0;

    _jacobian.resize(_numValues, nfree);
    _covariance.resize(nfree, nfree);
    _gsl->fdfParams = gsl_multifit_nlinear_default_parameters();

    // allocate initial paramter values and weights
    _gsl->x = gsl_vector_alloc(nfree);
    _gsl->wt = gsl_vector_alloc(_numValues);
    _gsl->covariance = gsl_matrix_alloc(nfree, nfree);

    // initialize the weights to 1
    for (size_t i = 0; i < _numValues; ++i)
        gsl_vector_set(_gsl->wt, i, 1.0);

    _gsl->workspace =
        gsl_multifit_nlinear_alloc(gsl_multifit_nlinear_trust, &_gsl->fdfParams, _numValues, nfree);
}

bool Minimizer::fit(int max_iter)
{
    if (!_gsl->workspace || !_f)
        return false;

    const auto nfree = _params.nfree();

    // too few data points to fit
    if (_numValues < nfree)
        return false;

    // function which computes vector of residuals
    _gsl->fdf.f = &Minimizer::gsl_f_wrapper;
    _gsl->fdf.df = _df ? &Minimizer::gsl_df_wrapper : nullptr;
    _gsl->fdf.p = nfree; // number of parameters to be fit
    _gsl->fdf.n = _numValues; // number of residuals
    _gsl->fdf.params = this; // this is the data ptr which is passed to gsl_f_wrapper
    _gsl->fdf.fvv = nullptr; // not using geodesic acceleration

    if (_df) {
        _dfInputEigen.resize(nfree);
        _dfOutputEigen.resize(_numValues, nfree);
    }

    // initialize solver with starting point and weights
    // note that this is NOT called in MinimizerGSL::initialize because _f needs
    // to be set first!
    _params.writeValues(_gsl->x);
    gslFromEigen(_wt, _gsl->wt);

    gsl_multifit_nlinear_winit(_gsl->x, _gsl->wt, &_gsl->fdf, _gsl->workspace);
    _gsl->status = gsl_multifit_nlinear_driver(
        max_iter, _xtol, _gtol, _ftol, &callback_helper, this, &_gsl->info, _gsl->workspace);
    gsl_multifit_nlinear_covar(_gsl->workspace->J, 1e-10, _gsl->covariance);

    eigenFromGSL(_gsl->workspace->J, _jacobian);
    _params.setValues(_gsl->workspace->x);
    eigenFromGSL(_gsl->covariance, _covariance);

    return _gsl->status == GSL_SUCCESS;
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

const char* Minimizer::getStatusStr()
{
    return gsl_strerror(_gsl->status);
}

int Minimizer::gsl_f_wrapper(const gsl_vector* input, void* data, gsl_vector* output)
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

void eigenFromGSL(const gsl_matrix* in, Eigen::MatrixXd& out)
{
    assert(int(in->size1) == out.rows());
    assert(int(in->size2) == out.cols());

    for (unsigned int i = 0; i < in->size1; ++i)
        for (unsigned j = 0; j < in->size2; ++j)
            out(i, j) = gsl_matrix_get(in, i, j);
}

static void gslFromEigen(const Eigen::VectorXd& in, gsl_vector* out)
{
    assert(in.size() == int(out->size));

    for (unsigned int i = 0; i < out->size; ++i)
        gsl_vector_set(out, i, in(i));
}

void gslFromEigen(const Eigen::MatrixXd& in, gsl_matrix* out)
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
    return K * _covariance * K.transpose();
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
    return _jacobian * K.transpose();
}

void Minimizer::setWeights(const Eigen::VectorXd& wt)
{
    assert(_numValues == wt.size());
    _wt = wt;
}

double Minimizer::meanSquaredError() const
{
    const gsl_vector* residual = _gsl->workspace->f;
    const int size = residual->size;
    double mse = 0.0;

    for (int i = 0; i < size; ++i) {
        const double ri = gsl_vector_get(residual, i);
        mse += ri * ri;
    }
    return mse / (size - _params.nfree());
}

} // namespace nsx
