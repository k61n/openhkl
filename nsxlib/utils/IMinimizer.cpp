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

#include "IMinimizer.h"


namespace SX {

namespace Utils {


IMinimizer::IMinimizer():
    _numValues(0),
    _numParams(0),
    _f(nullptr),
    _xtol(1e-7),
    _gtol(1e-7),
    _ftol(1e-7)
{
}

IMinimizer::~IMinimizer()
{
    deinitialize();
}

void IMinimizer::initialize(int params, int values)
{
    deinitialize();

    _numParams = params;
    _numValues = values;

    _x.resize(_numParams);
    _wt.resize(_numValues);

    for (int i = 0; i < _numValues; ++i)
        _wt(i) = 1.0;

    _jacobian.resize(_numValues, _numParams);
    _covariance.resize(_numParams, _numParams);
}


void IMinimizer::deinitialize()
{
    _f = nullptr;
}

void IMinimizer::setParams(const Eigen::VectorXd &x)
{
    assert(_numParams == x.size());
    _x = x;
}

void IMinimizer::setWeights(const Eigen::VectorXd &wt)
{
    assert(_numValues == wt.size());
    _wt = wt;
}

Eigen::MatrixXd IMinimizer::covariance()
{
    return _covariance;
}

Eigen::MatrixXd IMinimizer::jacobian()
{
    return _jacobian;
}

Eigen::VectorXd IMinimizer::params()
{
    return _x;
}

void IMinimizer::setxTol(double xtol)
{
    _xtol = xtol;
}

void IMinimizer::setgTol(double gtol)
{
    _gtol = gtol;
}

void IMinimizer::setfTol(double ftol)
{
    _ftol = ftol;
}

int IMinimizer::numIterations()
{
    return _numIter;
}


} // namespace Utils

} // namespace SX
