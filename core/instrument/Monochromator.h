//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Monochromator.h
//! @brief     Defines class Monochromator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INSTRUMENT_MONOCHROMATOR_H
#define NSX_CORE_INSTRUMENT_MONOCHROMATOR_H

#include "base/geometry/ReciprocalVector.h"

#include <yaml-cpp/yaml.h>

namespace ohkl {

//! Holds properties of a monochromator, used by Source.

class Monochromator {
 public:
    Monochromator();
    Monochromator(const std::string& name);
    Monochromator(const YAML::Node& node);

    //! Returns the name for this monochromator
    const std::string& name() const;

    //! Sets the name for this monochromator
    void setName(const std::string& name);

    //! Returns the wavelength for this monochromator
    double wavelength() const;

    //! Sets the wavelength for this monochromator
    void setWavelength(double wavelength);

    //! Returns the full width at half maximum of the wavelength for this
    //! monochromator
    double fullWidthHalfMaximum() const;

    //! Sets the full width at half maximum of the wavelength for this
    //! monochromator
    void setFullWidthHalfMaximum(double fwhm);

    //! Returns the width for this monochromator
    double width() const;

    //! Sets the width for this monochromator
    void setWidth(double width);

    //! Returns height for this monochromator
    double height() const;

    //! Sets the height for this monochromator
    void setHeight(double height);

    bool operator==(const Monochromator& other);

    //! Gets the incoming wave vector
    ReciprocalVector ki() const;

 private:
    std::string _name;
    double _wavelength;
    double _fwhm;
    double _width;
    double _height;
};

} // namespace ohkl

#endif // NSX_CORE_INSTRUMENT_MONOCHROMATOR_H
