/*
 * Monochromator.h
 *
 *  Created on: Apr 8, 2016
 *      Author: pellegrini
 */

#pragma once

#include "ReciprocalVector.h"

#include <string>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>

namespace nsx {

class Monochromator {

public:

	Monochromator();

	Monochromator(const std::string& name);

	~Monochromator();

	Monochromator(const Monochromator& other);

	Monochromator(const YAML::Node& node);

	Monochromator& operator=(const Monochromator& other);

	//! Return the name for this monochromator
	const std::string& name() const;

	//! Set the name for this monochromator
	void setName(const std::string& name);

	//! Return the wavelength for this monochromator
	double wavelength() const;

	//! Set the wavelength for this monochromator
	void setWavelength(double wavelength);

	//! Return the full width at half maximum of the wavelength for this monochromator
	double fullWidthHalfMaximum() const;

	//! Set the full width at half maximum of the wavelength for this monochromator
	void setFullWidthHalfMaximum(double fwhm);

	//! Return the width for this monochromator
	double width() const;

	//! Set the width for this monochromator
	void setWidth(double width);

	//! Return height for this monochromator
	double height() const;

	//! Set the height for this monochromator
	void setHeight(double height);

    bool operator==(const Monochromator& other);

    //! Get the incoming wave vector
    ReciprocalVector ki() const;

private:

	std::string _name;

	double _wavelength;

	double _fwhm;

	double _width;

	double _height;

};

} // end namespace nsx
