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

#ifndef NSXTOOL_COMPONENTSTATE_H_
#define NSXTOOL_COMPONENTSTATE_H_

#include <vector>
#include <Eigen/Geometry>

namespace SX {
namespace Instrument{

// Forward declare
class Component;

//! Maintain a state of a goniometer, following the memento pattern.
class ComponentState {
public:
    ComponentState();
    //! Copy constructor
    ComponentState(const ComponentState& other);
    //! Destructor
    ~ComponentState();
    //! Assignment operator
    ComponentState& operator=(const ComponentState& other);
    //! Return a pointer to the component related to this component state
     Component* getParent();
    const std::vector<double>& getValues() const;
     void setParent(Component*);
private:
    //! Only Component class can create a state

    //! Component must be able to access ComponentState
    friend class Component;
    //! Pointer to the Component that has created the state
    Component* _ptrComp;
    //! Values for each axis of the Component
    std::vector<double> _values;
};

}
}
#endif /* NSXTOOL_GONIOSTATE_H_ */
