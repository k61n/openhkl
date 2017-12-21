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
#include <cstring>
#include <iostream>

#include "Component.h"
#include "ComponentState.h"
#include "Gonio.h"

namespace nsx {

ComponentState::ComponentState(const Component* parent, std::vector<double> values):
    _ptrComp(parent),
    _rawValues(values.size()),
    _offsets(values.size())
{
    if (_ptrComp && _ptrComp->hasGonio()) {
        assert(values.size() == _ptrComp->getGonio()->getNPhysicalAxes());
    }

    for (auto i = 0; i < values.size(); ++i) {
        _rawValues(i) = values[i];
        _offsets(i) = 0.0;
    }
}

ComponentState::ComponentState(const ComponentState& other) : _ptrComp(other._ptrComp), _rawValues(other._rawValues), _offsets(other._offsets)
{
}

ComponentState::~ComponentState()
{
}

ComponentState& ComponentState::operator=(const ComponentState& other)
{
    if (this != &other) {
        _ptrComp = other._ptrComp;
        _rawValues = other._rawValues;
        _offsets = other._offsets;
    }
    return *this;
}

Eigen::ArrayXd ComponentState::values() const
{
    return _rawValues + _offsets;
}

Eigen::Vector3d ComponentState::getPosition() const
{
    auto gonio = _ptrComp->getGonio();
    auto position = _ptrComp->getRestPosition();

    if (gonio == nullptr) {
        return position;
    }

    gonio->transformInPlace(position, *this);

    return position;
}

ComponentState ComponentState::interpolate(const ComponentState &other, double t) const
{
    t = std::max(t, 0.0);
    t = std::min(t, 1.0);

    ComponentState result(*this);

    result._rawValues += t*(other._rawValues - _rawValues);
    result._offsets += t*(other._offsets - _offsets);

    return result;
}

} // End namespace nsx
