#include <memory>

#include <boost/foreach.hpp>

#include <Eigen/Dense>

#include "../kernel/Error.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "Sample.h"
#include "TransAxis.h"

namespace nsx {

Sample* Sample::create(const proptree::ptree& node)
{
    return new Sample(node);
}

Sample::Sample() : Component("sample"), _sampleShape(), _cells()
{
}

Sample::Sample(const Sample& other): Component(other), _sampleShape(other._sampleShape)
{
}

Sample::Sample(const std::string& name): Component(name), _sampleShape()
{
}

Sample::Sample(const proptree::ptree& node): Component(node)
{
}

Sample* Sample::clone() const
{
    return new Sample(*this);
}

Sample::~Sample()
{
}

Sample& Sample::operator=(const Sample& other)
{
    if (this != &other) {
        Component::operator=(other);
        _sampleShape = other._sampleShape;
    }
    return *this;
}

void Sample::setShape(const ConvexHull<double>& shape)
{
    _sampleShape = shape;
}

ConvexHull<double>& Sample::getShape()
{
    return _sampleShape;
}

std::shared_ptr<UnitCell> Sample::addUnitCell()
{
    _cells.push_back(std::shared_ptr<UnitCell>(new UnitCell()));
    return (_cells.back());
}

std::shared_ptr<UnitCell> Sample::getUnitCell(int i)
{
    if (i >= _cells.size()) {
        throw std::runtime_error("Unit Cell not valid");
    }
    return _cells[i];
}

const CellList& Sample::getUnitCells() const
{
    return _cells;
}

std::size_t Sample::getNCrystals() const
{
    return _cells.size();
}

void Sample::removeUnitCell(std::shared_ptr<UnitCell> cell)
{
    for (auto it = _cells.begin(); it != _cells.end(); ++it) {
        if ( *it == cell) {
            it = _cells.erase(it);
            break;
        }
    }
}

void Sample::removeUnitCell(int i)
{
    if (i < 0 || i >= _cells.size()) {
        return;
    }
    _cells.erase(_cells.begin()+i);
}

unsigned int Sample::getZ(int index) const
{
    if (index < 0 || index >= _cells.size()) {
        throw Error<Sample>("Invalid unit cell index.");
    }
    return _cells[index]->getZ();
}

void Sample::setZ(int Z, int index)
{
    if (index < 0 || index >= _cells.size()) {
        throw Error<Sample>("Invalid unit cell index.");
    }

    if (Z==0) {
        throw Error<Sample>("Invalid Z value.");
    }
    _cells[index]->setZ(Z);
}

sptrMaterial Sample::getMaterial(int index) const
{
    if (index < 0 || index >= _cells.size()) {
        throw Error<Sample>("Invalid unit cell index.");
    }
    return _cells[index]->getMaterial();
}

void Sample::setMaterial(sptrMaterial material, int index)
{
    if (index < 0 || index >= _cells.size()) {
        throw Error<Sample>("Invalid unit cell index.");
    }
    _cells[index]->setMaterial(material);
}

} // end namespace nsx

