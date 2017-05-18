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

#include "ComponentState.h"
#include "Component.h"
#include "Gonio.h"
#include <cstring>
#include <cassert>
#include <iostream>

namespace nsx {

ComponentState::ComponentState(const Component* parent, std::vector<double> values):
    _ptrComp(parent),
    _values(std::move(values))
{
    if (_ptrComp && _ptrComp->hasGonio()) {
        assert(_values.size() == _ptrComp->getGonio()->getNPhysicalAxes());
    }
}

ComponentState::ComponentState(const ComponentState& other) : _ptrComp(other._ptrComp), _values(other._values)
{
}

ComponentState::~ComponentState()
{
}

ComponentState& ComponentState::operator=(const ComponentState& other)
{
    if (this != &other) {
        _ptrComp = other._ptrComp;
        _values = other._values;
    }
    return *this;
}

const std::vector<double>& ComponentState::getValues() const
{
    return _values;
}

Eigen::Vector3d ComponentState::getPosition() const
{
    auto gonio = _ptrComp->getGonio();
    auto position = _ptrComp->getRestPosition();

    if (gonio == nullptr) {
        return position;
    }

    gonio->transformInPlace(position, _values);

    return position;
}

Eigen::Vector3d ComponentState::transformQ(const Eigen::Vector3d &q) const
{
    return _ptrComp->getGonio()->getInverseHomMatrix(_values).rotation()*q;
}

} // End namespace nsx
