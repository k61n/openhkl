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

#ifndef NSXTOOL_MODIFIER_H_
#define NSXTOOL_MODIFIER_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "XMLConfigurable.h"

namespace SX
{

namespace Geometry
{

class PrimitiveTransformation;

} // end namespace Geometry

namespace Instrument
{

using boost::property_tree::ptree;

using Eigen::Affine;
using Eigen::Transform;
using Eigen::Matrix3d;
using Eigen::Vector3d;

using SX::Geometry::PrimitiveTransformation;
using SX::Kernel::XMLConfigurable;

typedef Transform<double,3,3,Affine> HomMatrix;

/*
 * Defines a component modifier. A modifier object will act upon the position and oritentation of any instrument component.
 */
class Modifier : public XMLConfigurable
{

public:

	//! Default constructor.
	Modifier();
	//! Constructor from an XML node.
	Modifier(const ptree& node);
	//! Destructor.
	~Modifier();

	//! Add a new primitive transformation.
	void addTransformation(PrimitiveTransformation*);
	//! Returns the rotation part of the homogeneous matrix build from the composition of the primitive transformations.
	Matrix3d getRotation(const std::vector<double>&) const;
	//! Returns the homogeneous matrix build from the composition of the primitive transformations.
	HomMatrix getTransformation(const std::vector<double>&) const;
	//! Returns the translation part of the homogeneous matrix build from the composition of the primitive transformations.
	Vector3d getTranslation(const std::vector<double>&) const;
	//! Parse an XML node for a Modifier object.
	void parse(const ptree&);

private:
	std::vector<PrimitiveTransformation*> _trans;
	std::string _name;

	void _parse(const ptree&);

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_MODIFIER_H_ */
