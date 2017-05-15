/*
 * Monochromator.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: pellegrini
 */

#include "Monochromator.h"
#include "../utils/Units.h"

namespace nsx
{

namespace Instrument
{

Monochromator::Monochromator()
: _name(""),
  _wavelength(1.0),
  _offset(0.0),
  _offsetFixed(true),
  _fwhm(0.1),
  _width(0.01),
  _height(0.01)
{
}

Monochromator::Monochromator(const std::string& name)
: _name(name),
  _wavelength(1.0),
  _offset(0.0),
  _offsetFixed(true),
  _fwhm(0.1),
  _width(0.01),
  _height(0.01)
{
}

Monochromator::Monochromator(const Monochromator& other)
: _name(other._name),
  _wavelength(other._wavelength),
  _offset(other._offset),
  _offsetFixed(other._offsetFixed),
  _fwhm(other._fwhm),
  _width(other._width),
  _height(other._height)
{
}

Monochromator::Monochromator(const proptree::ptree& node)
: _offset(0.0),
  _offsetFixed(true)
{
    _name=node.get<std::string>("name");
    _wavelength=node.get<double>("wavelength");
    _fwhm=node.get<double>("fwhm");
    Units::UnitsManager* um=nsx::Units::UnitsManager::Instance();

    // Set the source slit width from the property tree node
    const proptree::ptree& widthNode = node.get_child("width");
    double units=um->get(widthNode.get<std::string>("<xmlattr>.units"));
    _width=widthNode.get_value<double>();
    _width *= units;

    // Set the source slit height from the property tree node
    const proptree::ptree& heightNode = node.get_child("height");
    units=um->get(heightNode.get<std::string>("<xmlattr>.units"));
    _height=heightNode.get_value<double>();
    _height *= units;

}

Monochromator& Monochromator::operator=(const Monochromator& other)
{
    if (this!=&other)
    {
        _name = other._name;
        _wavelength = other._wavelength;
        _offset = other._offset;
        _offsetFixed = other._offsetFixed;
        _fwhm = other._fwhm;
        _width = other._width;
        _height = other._height;
    }
    return *this;
}

Monochromator::~Monochromator()
{
}

const std::string& Monochromator::getName() const
{
    return _name;
}

void Monochromator::setName(const std::string& name)
{
    _name = name;
}

double Monochromator::getWavelength() const
{
    return _wavelength + _offset;
}

void Monochromator::setWavelength(double wavelength)
{
    _wavelength = wavelength;
}

double Monochromator::getFWHM() const
{
    return _fwhm;
}

void Monochromator::setFWHM(double fwhm)
{
    _fwhm = fwhm;
}

double Monochromator::getWidth() const
{
    return _width;
}

void Monochromator::setWidth(double width)
{
    _width = width;
}

double Monochromator::getHeight() const
{
    return _height;
}

void Monochromator::setHeight(double height)
{
    _height = height;
}

double Monochromator::getOffset() const
{
    return _offset;
}

void Monochromator::setOffset(double offset)
{
    if (_offsetFixed)
        return;
    _offset = offset;
}

void Monochromator::setOffsetFixed(bool offsetFixed)
{
    _offsetFixed = offsetFixed;
}

bool Monochromator::isOffsetFixed() const
{
    return _offsetFixed;
}

bool Monochromator::operator==(const Monochromator& other)
{
    if (_name != other._name)
        return false;

    if (_wavelength != other._wavelength)
        return false;

    if (_offset != other._offset)
        return false;

    if (_offsetFixed != other._offsetFixed)
        return false;

    if (_fwhm != other._fwhm)
        return false;

    if ( _width != other._width)
        return false;

    if ( _height != other._height)
        return false;

    return true;
}

Eigen::Vector3d Monochromator::getKi() const
{
    return Eigen::Vector3d(0,1.0/_wavelength,0.0);
}

} /* namespace Instrument */

} // end namespace nsx
