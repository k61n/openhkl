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

#ifndef NSXTOOL_COMPONENT_H_
#define NSXTOOL_COMPONENT_H_
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "XMLConfigurable.h"
#include <memory>
#include <Eigen/Dense>
#include "ComponentState.h"

namespace SX
{

namespace Instrument
{

class Gonio;

using boost::property_tree::ptree;
using SX::Kernel::XMLConfigurable;

typedef unsigned int uint;

/*
 * Interface for the components of an instrument (e.g. detector, goniometer, source ...).
 */
class Component : public XMLConfigurable
{
public:
	//! Default constructor
	Component();
	//! Constructs a component with a given name
	Component(const std::string& name);
	// Copy constructor
	Component(const Component& other);
	//! Assignment operator
	virtual Component& operator=(const Component& other);
	//! Destructor.
	virtual ~Component()=0;
	//! Virtual copy constructor
	virtual Component* clone() const=0;
	//! Returns the name of the component.
	const std::string& getName() const;
	//! Parse the XML component node.
	virtual void parse(const ptree& pt);
	//! Attach a modifier to the component.
	void setGonio(std::shared_ptr<Gonio>);
	//! Return true if a geometryic modifier is attached
	bool hasGonio() const;
	//! Return the goniometer attached to this component
	std::shared_ptr<Gonio> getGonio() const;
	//! Set the rest position
	virtual void setRestPosition(const Eigen::Vector3d& pos);
	//! Get the absolute position at rest (unmodified by gonio)
	const Eigen::Vector3d& getRestPosition() const;
	//! Get the absolute position of the component for a set of goniometer values
	Eigen::Vector3d getPosition(const std::vector<double>& goniosetup) const;
	Eigen::Vector3d getPosition(const ComponentState& state) const;
	//! Create a state
	ComponentState createState(const std::vector<double>& values);
	//! Return the number of axes attached to this component
	std::size_t nAxes() const;
protected:
	//! Name of the component
	std::string _name;
	//! Pointer to the goniometer attached to the component
	std::shared_ptr<Gonio> _gonio;
	//! The position of the component at rest, i.e. not modified by the Gonio.
	Eigen::Vector3d _position;

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_COMPONENT_H_ */
