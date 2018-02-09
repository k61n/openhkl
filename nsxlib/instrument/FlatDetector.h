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

#include <yaml-cpp/yaml.h>

#include "GeometryTypes.h"
#include "MonoDetector.h"

namespace nsx {

class FlatDetector : public MonoDetector {
public:

	//! Static constructor of a FlatDetector from a property tree node
	static Detector* create(const YAML::Node& node);

	//! Construct a FlatDetector
	FlatDetector();
	//! Construct a FlatDetector from another one
	FlatDetector(const FlatDetector& other);
	//! Construct a FlatDetector with a given name
	FlatDetector(const std::string& name);
	//! Constructs a FlatDetector from a property tree node
	FlatDetector(const YAML::Node& node);
	//! Return a pointer to a copy of a FlatDetector
	Detector* clone() const override;
	//! Destructor
	virtual ~FlatDetector();

	//! Assignment operator
	FlatDetector& operator=(const FlatDetector& other);

	//! Set the height of the detector (meters)
	void setHeight(double height) override;
	//! Set the width of the detector (meters)
	void setWidth(double width) override;

	//! Set the angular height of the detector (radians)
	void setAngularHeight(double angle) override;
	//! Set the angular width of the detector (radians)
	void setAngularWidth(double angle) override;

	//! Returns the position of a given pixel in detector space. This takes into account the detector motions in detector space.
	DirectVector pixelPosition(double px,double py) const override;

	//! Determine whether detector at rest can receive a scattering event with direction given by Kf. px and py are detector position if true.
	virtual Eigen::Vector3d constructEvent(const DirectVector& from, const ReciprocalVector& kf) const override;

};

} // end namespace nsx
