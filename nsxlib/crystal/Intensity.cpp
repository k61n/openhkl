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

#include "Intensity.h"
#include <cmath>
#include <cassert>

namespace nsx {

Intensity::Intensity(double value, double sigma2): _value(value), _sigma2(sigma2)
{
    if (_sigma2 < 0.0 || std::isnan(_sigma2)) {
        _sigma2 = 0.0;
    }
}

Intensity::Intensity(const Intensity& other): _value(other._value), _sigma2(other._sigma2)
{
    if (_sigma2 < 0.0 || std::isnan(_sigma2)) {
        _sigma2 = 0.0;
    }
}

double Intensity::value() const
{
    return _value;
}

double Intensity::sigma() const
{
    return std::sqrt(_sigma2);
}

Intensity Intensity::operator+(const Intensity &other) const
{
    return Intensity(_value + other._value, _sigma2 + other._sigma2);
}

Intensity Intensity::operator-(const Intensity &other) const
{
    return Intensity(_value - other._value, _sigma2 + other._sigma2);
}

Intensity Intensity::operator*(double scale) const
{
    if (scale < 0.0 || std::isnan(scale)) {
        return Intensity(0.0, 0.0);
    }
    return Intensity(scale*_value, scale*scale*_sigma2);
}

Intensity Intensity::operator/(double denominator) const
{
    const double scale = 1.0 / denominator;
    return Intensity(scale*_value, scale*scale*_sigma2);
}

Intensity& Intensity::operator+=(const Intensity& other)
{
    _value += other._value;
    _sigma2 += other._sigma2;
    return *this;
}

} // end namespace nsx
