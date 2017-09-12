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

#include "FitParameters.h"

namespace nsx {

int FitParameters::addParameter(double* addr)
{
    _params.emplace_back(addr);
    return _params.size()-1;
}


void FitParameters::setValues(const gsl_vector* v)
{
    const size_t size = _params.size();
    assert(v->size == size);

    for (size_t i = 0; i < size; ++i) {
        *_params[i] = gsl_vector_get(v, i);
    }
}

void FitParameters::writeValues(gsl_vector* v) const
{
    const size_t size = _params.size();
    assert(v->size == size);

    for (size_t i = 0; i < size; ++i) {
        gsl_vector_set(v, i, *_params[i]);
    }
}

size_t FitParameters::size() const
{
    return _params.size();
}

} // end namespace nsx
