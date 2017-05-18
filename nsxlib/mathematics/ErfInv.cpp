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

#include <cmath>
#include <iostream>
#include <limits>

#include "../mathematics/ErfInv.h"

namespace nsx {

static double erf_inv_approx(double y);

// inverse error function via newton method
// this is slow but it's never used in time-critical code
double erf_inv(double y)
{
    static const double fact = 1.0 / (2.0 / std::sqrt(M_PI));
    const std::size_t count = 5;

    if (y >= 1 || y <= -1) {
        return std::nan("");
    }

    // get initial approximation
    double x = erf_inv_approx(y);
    size_t i = 0;

    // improve by taking several steps of newton's method
    for(auto i = 0; i < count; ++i) {
        const double num = y - std::erf(x);
        const double den_inv = fact * std::exp(x*x);
        const double dx = num * den_inv;
        x += dx;
    }
    return x;
}

// approximation taken from wikipedia:
// https://en.wikipedia.org/wiki/Error_function#Inverse_functions
double erf_inv_approx(double y)
{
    static const double a = 0.147;
    static const double pi = M_PI;

    const double sign = y < 0? -1.0 : 1.0;
    const double logy = std::log(1.0 - y*y);
    const double A = 2.0 / pi / a + logy / 2.0;
    const double B = logy / a;

    const double C = std::sqrt(A*A-B);
    const double D = std::sqrt(C-A);

    return sign*D;
}

double getScale(double confidence)
{
    static const double factor = std::sqrt(2.0);
    return factor * erf_inv(confidence);
}

double getConfidence(double x)
{
    static const double factor = 1.0 / std::sqrt(2.0);
    return std::erf(x*factor);
}

} // end namespace nsx
