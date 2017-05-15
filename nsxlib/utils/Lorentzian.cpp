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

#include "Lorentzian.h"
#include <cmath>

static const double g_pi = double(M_PI);

namespace nsx {
namespace Utils {

Lorentzian::Lorentzian(double a, double b, double x0):
    _a(a), _b(b), _x0(x0)
{

}

double Lorentzian::evaluate(double x) const
{
    const double u = x - _x0;
    return _a*_a / (u*u + _b*_b);
}

double Lorentzian::integrate() const
{
    return _a*_a * g_pi / _b;
}

double nsx::Utils::Lorentzian::evaluateScan(double x) const
{
    const double t2 = (x - _x0) / _b;
    const double t1 = (x - _x0-1.0) / _b;
    return _a*_a/_b*(std::atan(t2) - std::atan(t1));
}

} // namespace Utils
} // namespace nsx
