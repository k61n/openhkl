//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Monochromator.cpp
//! @brief     Implements class Monochromator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/Monochromator.h"
#include "base/utils/Units.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

Monochromator::Monochromator() : _wavelength(1.0), _fwhm(0.1), _width(0.01), _height(0.01) { }

Monochromator::Monochromator(const std::string& name)
    : _name(name), _wavelength(1.0), _fwhm(0.1), _width(0.01), _height(0.01)
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
}

const std::string& Monochromator::name() const
{
    return _name;
}

void Monochromator::setName(const std::string& name)
{
    _name = name;
}

double Monochromator::wavelength() const
{
    return _wavelength;
}

void Monochromator::setWavelength(double wavelength)
{
    _wavelength = wavelength;
}

double Monochromator::fullWidthHalfMaximum() const
{
    return _fwhm;
}

void Monochromator::setFullWidthHalfMaximum(double fwhm)
{
    _fwhm = fwhm;
}

double Monochromator::width() const
{
    return _width;
}

void Monochromator::setWidth(double width)
{
    _width = width;
}

double Monochromator::height() const
{
    return _height;
}

void Monochromator::setHeight(double height)
{
    _height = height;
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
