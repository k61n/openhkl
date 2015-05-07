/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
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

#ifndef NSXTOOL_SOURCE_H_
#define NSXTOOL_SOURCE_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

#include "Component.h"

namespace SX
{

namespace Instrument
{

namespace property_tree=boost::property_tree;

class Source : public Component
{
public:

	static Source* create(const property_tree::ptree& node);

	// Default constructor
	Source();
	//! Copy constructor
	Source(const Source& other);
	//! Constructs a default source with a given name
	Source(const std::string& name);
	//! Destructor
	virtual ~Source();
	//! Virtual copy constructor
	Component* clone() const;

	//! Assignment operator
	Source& operator=(const Source& other);

	//! Construct a Source from an XML node
	void buildFromXML(const property_tree::ptree& node);

	double getWavelength() const;
	Eigen::Vector3d getki() const;
	void setWavelength(double wavelength);
	//! Set an offset in wavelength. No effect if _offsetFiexed is set to True.
	void setOffset(double offset);
	void setOffsetFixed(bool fixed);
	bool hasOffsetFixed() const;

	void setWidth(double width);
	double getWidth() const;

	void setHeight(double height);
	double getHeight() const;

private:
	double _wavelength;
	double _offset;
	bool _offsetFixed;
	double _width;
	double _height;
};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_SOURCE_H_ */
