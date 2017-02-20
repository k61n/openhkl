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

#include "Gaussian.h"
#include <cmath>

static const double g_pi = double(M_PI);

namespace SX {
namespace Utils {

Gaussian::Gaussian(double a, double mu, double sigma):
    _a(a), _mu(mu), _sigma(sigma)
{
}

double Gaussian::evaluate(double x) const
{
    const double z = (x-_mu) / _sigma;
    return _a*_a* std::exp(-z*z / 2.0);
}

double Gaussian::integrate() const
{
    return _a*_a * std::sqrt(2.0*g_pi*_sigma*_sigma);
}

} // namespace Utils
} // namespace SX
