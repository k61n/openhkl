/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include <cassert>

#include <Eigen/Dense>

#include "FitParameters.h"

namespace nsx {

int FitParameters::addParameter(double* addr)
{
    _params.emplace_back(addr);
    resetConstraints();
    return _params.size()-1;
}


void FitParameters::setValues(const gsl_vector* v)
{
    assert(v->size == nfree());
    Eigen::VectorXd p0(nfree());

    for (size_t i = 0; i < nfree(); ++i) {
        p0(i) = gsl_vector_get(v, i);
    }
    auto p1 = _K * p0;

    for (size_t i = 0; i < nparams(); ++i) {
        *_params[i] = p1(i);
    }
}

void FitParameters::writeValues(gsl_vector* v) const
{
    assert(v->size == nfree());

    for (size_t i = 0; i < nfree(); ++i) {
        gsl_vector_set(v, i, *_params[i]);
    }
}

size_t FitParameters::nfree() const
{
    return _params.size() == 0 ? 0 : _K.cols();
}

void FitParameters::setConstraint(const Eigen::MatrixXd& C)
{
    _K = Eigen::FullPivLU<Eigen::MatrixXd>(C).kernel();
    _P = (_K.transpose()*_K).inverse() * _K.transpose();
}

void FitParameters::resetConstraints()
{
    const auto n = nparams();

    if (n > 0) {
        _K.setIdentity(n, n);
        _P.setIdentity(n, n);
    }
}

size_t FitParameters::nparams() const
{
    return _params.size();
}

} // end namespace nsx
