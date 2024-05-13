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

Monochromator::Monochromator()
    : _wavelength(1.0), _fwhm(0.1), _width(0.01), _height(0.01), _x_offset(0.0), _y_offset(0.0)
{
}

Monochromator::Monochromator(const std::string& name)
    : _name(name)
    , _wavelength(1.0)
    , _fwhm(0.1)
    , _width(0.01)
    , _height(0.01)
    , _x_offset(0.0)
    , _y_offset(0.0)
{
}

Monochromator::Monochromator(const YAML::Node& node)
{
    _name = node[ohkl::ym_instrumentName].as<std::string>();

    // Sets the source slit width from the yaml tree node
    auto&& widthNode = node[ohkl::ym_width];
    _width = widthNode[ohkl::ym_value].as<double>()
        * UnitsManager::get(widthNode[ohkl::ym_units].as<std::string>());

    // Sets the source slit height from the yaml tree node
    auto&& heightNode = node[ohkl::ym_height];
    _height = heightNode[ohkl::ym_value].as<double>()
        * UnitsManager::get(heightNode[ohkl::ym_units].as<std::string>());

    auto&& wavelengthNode = node[ohkl::ym_wavelength];
    _wavelength = wavelengthNode[ohkl::ym_value].as<double>()
        * UnitsManager::get(wavelengthNode[ohkl::ym_units].as<std::string>()) / ohkl::ang;

    auto&& fwhmNode = node[ohkl::ym_fwhm];
    _fwhm = fwhmNode[ohkl::ym_value].as<double>()
        * UnitsManager::get(fwhmNode[ohkl::ym_units].as<std::string>()) / ohkl::ang;

    auto&& xoffsetNode = node[ohkl::ym_xoffset];
    _x_offset = xoffsetNode[ohkl::ym_value].as<double>();

    auto&& yoffsetNode = node[ohkl::ym_yoffset];
    _y_offset = yoffsetNode[ohkl::ym_value].as<double>();
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
