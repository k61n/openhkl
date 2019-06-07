//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Monochromator.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_MONOCHROMATOR_H
#define CORE_INSTRUMENT_MONOCHROMATOR_H

#include "ReciprocalVector.h"

#include <string>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>

namespace nsx {

//! Class describing a monochromator together with various of its properties.
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

    //! Return the full width at half maximum of the wavelength for this
    //! monochromator
    double fullWidthHalfMaximum() const;

    //! Set the full width at half maximum of the wavelength for this
    //! monochromator
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

#endif // CORE_INSTRUMENT_MONOCHROMATOR_H
