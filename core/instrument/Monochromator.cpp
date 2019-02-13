/*
 * Monochromator.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: pellegrini
 */

#include "Monochromator.h"
#include "Units.h"

namespace nsx {

Monochromator::Monochromator()
: _name(""),
  _wavelength(1.0),
  _fwhm(0.1),
  _width(0.01),
  _height(0.01)
{
}

Monochromator::Monochromator(const std::string& name)
: _name(name),
  _wavelength(1.0),
  _fwhm(0.1),
  _width(0.01),
  _height(0.01)
{
}

Monochromator::Monochromator(const Monochromator& other)
: _name(other._name),
  _wavelength(other._wavelength),
  _fwhm(other._fwhm),
  _width(other._width),
  _height(other._height)
{
}

Monochromator::Monochromator(const YAML::Node& node)
{
    _name = node["name"].as<std::string>();

    UnitsManager* um=UnitsManager::Instance();

    // Set the source slit width from the yaml tree node
    auto&& widthNode = node["width"];
    _width = widthNode["value"].as<double>() * um->get(widthNode["units"].as<std::string>());

    // Set the source slit height from the yaml tree node
    auto&& heightNode = node["height"];
    _height = heightNode["value"].as<double>() * um->get(heightNode["units"].as<std::string>());

    auto&& wavelengthNode = node["wavelength"];
    _wavelength = wavelengthNode["value"].as<double>() * um->get(wavelengthNode["units"].as<std::string>()) / nsx::ang;

    auto&& fwhmNode = node["fwhm"];
    _fwhm = fwhmNode["value"].as<double>() * um->get(fwhmNode["units"].as<std::string>()) / nsx::ang;
}

Monochromator& Monochromator::operator=(const Monochromator& other)
{
    if (this!=&other) {
        _name = other._name;
        _wavelength = other._wavelength;      
        _fwhm = other._fwhm;
        _width = other._width;
        _height = other._height;
    }
    return *this;
}

Monochromator::~Monochromator()
{
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

    if ( _width != other._width)
        return false;

    if ( _height != other._height)
        return false;

    return true;
}

ReciprocalVector Monochromator::ki() const
{
    return ReciprocalVector(0.0, 1.0/_wavelength, 0.0);
}

} // end namespace nsx
