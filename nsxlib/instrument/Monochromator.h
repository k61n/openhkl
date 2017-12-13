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

    bool operator==(const Monochromator& other);

    //! Get the incoming wave vector
    ReciprocalVector getKi() const;

private:

	std::string _name;

	double _wavelength;

	double _fwhm;

	double _width;

	double _height;

};

} // end namespace nsx
