//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/fit/FitParameters.cpp
//! @brief     Implements class FitParameters
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cassert>

#include <Eigen/Dense>
#include <Eigen/SparseQR>

#include "base/fit/FitParameters.h"

// DEBUGGING
#include <iostream>

namespace nsx {

int FitParameters::addParameter(double* addr)
{
    _params.emplace_back(addr);
    _originalValues.emplace_back(*addr);
    _constrained = false;
    return _params.size() - 1;
}

void FitParameters::setValues(const gsl_vector* v)
{
    assert(v->size == nfree());
    Eigen::VectorXd p0(nfree());

    for (size_t i = 0; i < nfree(); ++i)
        p0(i) = gsl_vector_get(v, i);

    const auto& p1 = _constrained ? _K * p0 : p0;

    for (size_t i = 0; i < nparams(); ++i) {
        *_params[i] = p1(i);
    }
}

void FitParameters::writeValues(gsl_vector* v) const
{
    assert(v->size == nfree());
    Eigen::VectorXd p1(nparams());

    for (size_t i = 0; i < _params.size(); ++i)
        p1(i) = *_params[i];

    const auto& p0 = _constrained ? _P * p1 : p1;

    for (size_t i = 0; i < nfree(); ++i)
        gsl_vector_set(v, i, p0(i));
}

size_t FitParameters::nfree() const
{
    if (!_constrained)
        return _params.size();
    return _params.empty() ? 0 : _K.cols();
}

void FitParameters::setConstraint(const Eigen::SparseMatrix<double>& C_input)
{
    Eigen::SparseMatrix<double> C(C_input);
    C.makeCompressed();

    // solver will factorize C as CU = QR with U a permutation, Q orthogonal, and
    // R triangluar
    using SolverType = Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>;
    SolverType solver1, solver2;

    solver1.analyzePattern(C);
    solver1.factorize(C);

    // rank and number of columns
    auto r = solver1.rank();
    auto n = solver1.cols();

    // Gets the permutation
    SolverType::PermutationType U = solver1.colsPermutation();
    // Gets R, and restrict so that the number of rows is equal to the rank
    Eigen::SparseMatrix<double> R = solver1.matrixR().topLeftCorner(r, n);

    // Write R in block form R = [R0 R1] where R0 is square and full rank
    Eigen::SparseMatrix<double> R0 = R.block(0, 0, r, r);
    Eigen::SparseMatrix<double> R1 = R.block(0, r, r, n - r);

    // now factorize R0
    solver2.analyzePattern(R0);
    solver2.factorize(R0);

    // Let K = [K0, I] be the kernel matrix for R.
    // Then setting R*K = 0 gives the equation R0*K0 = -R1 which we solve below
    Eigen::MatrixXd K0 = solver2.solve(-R1);

    Eigen::MatrixXd K;
    K.resize(n, n - r);
    K.block(0, 0, r, n - r) = K0;
    K.block(r, 0, n - r, n - r).setIdentity();

    // undo the permutation from QR pivoting
    setKernel(U * K);

    _constrained = true;
}

void FitParameters::resetConstraints()
{
    _constrained = false;
}

size_t FitParameters::nparams() const
{
    return _params.size();
}

const Eigen::MatrixXd& FitParameters::kernel() const
{
    static Eigen::MatrixXd id;

    if (_constrained)
        return _K;

    id.setIdentity(_params.size(), _params.size());
    return id;
}

void FitParameters::reset()
{
    for (size_t i = 0; i < _params.size(); ++i) {
        *_params[i] = _originalValues[i];
    }
}

void FitParameters::setKernel(const Eigen::MatrixXd& ker)
{
    _K = ker;
    _P = (_K.transpose() * _K).inverse() * _K.transpose();
    _constrained = true;
}

} // namespace nsx
