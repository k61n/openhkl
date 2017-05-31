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

#include "ChiSquared.h"
#include <cmath>
#include <gsl/gsl_sf_gamma.h>


namespace nsx {

ChiSquared::ChiSquared(double k): _k(k), _c(0.0)
{
    _c = std::pow(2.0, -_k/2) / gsl_sf_gamma(_k/2);
}

double ChiSquared::pdf(double x) const
{
    return _c * std::pow(x, _k/2-1.0) * std::exp(-x/2);
}

double ChiSquared::cdf(double x) const
{
    return gsl_sf_gamma_inc_P(_k/2, x/2);
}
    
} // end namespace nsx
