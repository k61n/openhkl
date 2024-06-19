//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Monochromator.h
//! @brief     Defines class Monochromator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_MONOCHROMATOR_H
#define OHKL_CORE_INSTRUMENT_MONOCHROMATOR_H

#include <yaml-cpp/yaml.h>

namespace ohkl {

class ReciprocalVector;

//! Holds properties of a monochromator, used by Source.

class Monochromator {
 public:
    Monochromator() = delete;
    Monochromator(const YAML::Node& node);

    //! Returns the name for this monochromator
    const std::string& name() const { return _name; };

    //! Sets the name for this monochromator
    void setName(const std::string& name) { _name = name; };

    //! Returns the wavelength for this monochromator
    double wavelength() const { return _wavelength; };

    //! Sets the wavelength for this monochromator
    void setWavelength(double wavelength) { _wavelength = wavelength; };

    //! Returns the full width at half maximum of the wavelength for this
    //! monochromator
    double fullWidthHalfMaximum() const { return _fwhm; };

    //! Sets the full width at half maximum of the wavelength for this
    //! monochromator
    void setFullWidthHalfMaximum(double fwhm) { _fwhm = fwhm; };

    //! Returns the width for this monochromator
    double width() const { return _width; };

    //! Sets the width for this monochromator
    void setWidth(double width) { _width = width; };

    //! Returns height for this monochromator
    double height() const { return _height; };

    //! Return beam offset in x-direction (pixels)
    double xOffset() const { return _x_offset; };

    //! Set beam offset in x-direction (pixels)
    void setXOffset(double offset) { _x_offset = offset; };

    //! Return beam offset in x-direction (pixels)
    double yOffset() const { return _y_offset; };

    //! Set beam offset in y-direction (pixels)
    void setYOffset(double offset) { _y_offset = offset; };

    //! Sets the height for this monochromator
    void setHeight(double height) { _height = height; };

    bool operator==(const Monochromator& other);

    //! Gets the incoming wave vector
    ReciprocalVector ki() const;

 private:
    //! Monochromator name
    std::string _name;
    //! Wavelength in Angstroem
    double _wavelength;
    //! Full width half maximum in Angstroem
    double _fwhm;
    //! Monochromator width
    double _width;
    //! Monochromator height
    double _height;
    //! Direct beam x offset in pixels
    double _x_offset;
    //! Direct beam y offset in pixels
    double _y_offset;
};

} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_MONOCHROMATOR_H
