/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

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

#pragma once

#include <vector>

#include <Eigen/Dense>

namespace nsx {

// Forward declare
class Component;
struct InstrumentState;

//! Maintain a state of a goniometer, following the memento pattern.
class ComponentState {
public:
    //! Constructor
    ComponentState(const Component* parent = nullptr, std::vector<double> values = {});
    //! Copy constructor
    ComponentState(const ComponentState& other);
    //! Destructor
    ~ComponentState();
    //! Assignment operator
    ComponentState& operator=(const ComponentState& other);
    //! Return the values (= raw values + offsets) of the axes
    Eigen::ArrayXd values() const;
    //void setParent(Component*);


    Eigen::Vector3d getPosition() const;

    ComponentState interpolate(const ComponentState &other, double t) const;

private:
    friend struct InstrumentState;
    //! Pointer to the Component that has created the state
    const Component* _ptrComp;
    //! Raw values for each axis of the component
    Eigen::ArrayXd _rawValues;     
    //! Offsets for the value of each axis of the component.
    Eigen::ArrayXd _offsets;
};

} // end namespace nsx
