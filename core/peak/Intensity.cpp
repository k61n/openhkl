//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/Intensity.cpp
//! @brief     Implements class Intensity
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/Intensity.h"

#include <cmath>

namespace ohkl {

Intensity::Intensity() : _value(0), _sigma2(0), _valid(false) { }

Intensity::Intensity(const double value, const double sigma2)
    : _value(value), _sigma2(sigma2), _valid(true)
{
    // Deal with invalid sigmas later so that we can diagnose the problem.
    if (std::isnan(value) || _sigma2 < 0.0 || std::isnan(_sigma2))
        _valid = false;
}

double Intensity::value() const
{
    return _value;
}

double Intensity::sigma() const
{
    return std::sqrt(_sigma2);
}

double Intensity::variance() const
{
    return _sigma2;
}

double Intensity::strength() const
{
    if ((std::abs(_value) < _eps) && (std::abs(_sigma2) < _eps))
        return 0.0;
    return _value / sigma();
}

Intensity Intensity::operator+(const Intensity& other) const
{
    return {_value + other.value(), _sigma2 + other._sigma2};
}

Intensity Intensity::operator-(const Intensity& other) const
{
    return {_value - other.value(), _sigma2 + other._sigma2};
}

Intensity& Intensity::operator*=(double s)
{
    _value *= s;
    _sigma2 *= s * s;
    return *this;
}

Intensity Intensity::operator*(double scale) const
{
    return {scale * _value, scale * scale * _sigma2};
}

Intensity Intensity::operator/(double denominator) const
{
    const double scale = 1.0 / denominator;
    return {scale * _value, scale * scale * _sigma2};
}

Intensity& Intensity::operator+=(const Intensity& other)
{
    _value += other.value();
    _sigma2 += other._sigma2;
    return *this;
}

Intensity Intensity::operator/(const Intensity& other) const
{
    const double d = 1.0 / other.value();
    return {_value * d, d * d * (_sigma2 + value() * other._sigma2)};
}

bool Intensity::isValid() const
{
    return _valid;
}

} // namespace ohkl
