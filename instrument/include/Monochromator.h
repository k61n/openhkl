/*
 * Monochromator.h
 *
 *  Created on: Apr 8, 2016
 *      Author: pellegrini
 */

#ifndef INSTRUMENT_INCLUDE_MONOCHROMATOR_H_
#define INSTRUMENT_INCLUDE_MONOCHROMATOR_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace SX
{

namespace Instrument
{

namespace proptree=boost::property_tree;

class Monochromator
{

public:

	Monochromator();

	Monochromator(const std::string& name);

	~Monochromator();

	Monochromator(const Monochromator& other);

	Monochromator(const proptree::ptree& node);

	Monochromator& operator=(const Monochromator& other);

	//! Return the name for this monochromator
	const std::string& getName() const;

	//! Set the name for this monochromator
	void setName(const std::string& name);

	//! Return the wavelength for this monochromator
	double getWavelength() const;

	//! Set the wavelength for this monochromator
	void setWavelength(double wavelength);

	//! Return the full width at half maximum of the wavelength for this monochromator
	double getFWHM() const;

	//! Set the full width at half maximum of the wavelength for this monochromator
	void setFWHM(double fwhm);

	//! Return the width for this monochromator
	double getWidth() const;

	//! Set the width for this monochromator
	void setWidth(double width);

	//! Return height for this monochromator
	double getHeight() const;

	//! Set the height for this monochromator
	void setHeight(double height);

	//! Return the offset of the wavelength of this monochromator
	double getOffset() const;

	//! Set the offset of the wavelength of this monochromator
	void setOffset(double offset);

	//! Fix the offset of the wavelength of this monochromator
	void setOffsetFixed(bool offsetFixed);

	//! Return whether the offset of the wavelength of this monochromator is fixed or not
	bool isOffsetFixed() const;

    bool operator==(const Monochromator& other);

private:

	std::string _name;

	double _wavelength;

	double _offset;

	double _offsetFixed;

	double _fwhm;

	double _width;

	double _height;

};

} /* namespace Instrument */

} /* namespace SX */

#endif /* INSTRUMENT_INCLUDE_MONOCHROMATOR_H_ */
