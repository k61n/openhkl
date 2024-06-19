//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Monochromator.cpp
//! @brief     Implements class Monochromator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/Monochromator.h"

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Units.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

Monochromator::Monochromator(const YAML::Node& node)
{
    _name = node[ym_instrumentName].as<std::string>();

    // Sets the source slit width from the yaml tree node
    auto&& widthNode = node[ym_width];
    _width = widthNode[ym_value].as<double>()
        * UnitsManager::get(widthNode[ym_units].as<std::string>());

    // Sets the source slit height from the yaml tree node
    auto&& heightNode = node[ym_height];
    _height = heightNode[ym_value].as<double>()
        * UnitsManager::get(heightNode[ym_units].as<std::string>());

    auto&& wavelengthNode = node[ym_wavelength];
    _wavelength = wavelengthNode[ym_value].as<double>()
        * UnitsManager::get(wavelengthNode[ym_units].as<std::string>()) / ang;

    auto&& fwhmNode = node[ym_fwhm];
    _fwhm = fwhmNode[ym_value].as<double>()
        * UnitsManager::get(fwhmNode[ym_units].as<std::string>()) / ang;

    auto&& xoffsetNode = node[ym_xoffset];
    _x_offset = xoffsetNode[ym_value].as<double>();

    auto&& yoffsetNode = node[ym_yoffset];
    _y_offset = yoffsetNode[ym_value].as<double>();
}


bool Monochromator::operator==(const Monochromator& other)
{
    if (_name != other._name)
        return false;

    if (_wavelength != other._wavelength)
        return false;

    if (_fwhm != other._fwhm)
        return false;

    if (_width != other._width)
        return false;

    if (_height != other._height)
        return false;

    return true;
}

ReciprocalVector Monochromator::ki() const
{
    return ReciprocalVector(0.0, 1.0 / _wavelength, 0.0);
}

} // namespace ohkl
