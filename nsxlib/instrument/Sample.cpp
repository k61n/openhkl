#include <stdexcept>

#include <Eigen/Dense>

#include "Gonio.h"
#include "RotAxis.h"
#include "Sample.h"
#include "TransAxis.h"
#include "UnitCell.h"

namespace nsx {

Sample* Sample::create(const YAML::Node& node)
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

Sample::Sample(const YAML::Node& node): Component(node)
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

void Sample::setShape(const ConvexHull& shape)
{
    _sampleShape = shape;
}

ConvexHull& Sample::shape()
{
    return _sampleShape;
}

sptrUnitCell Sample::addUnitCell(sptrUnitCell cell)
{
    if (cell == nullptr) {
        cell = std::make_shared<UnitCell>(UnitCell());
    }
    _cells.push_back(cell);
    return cell;
}

sptrUnitCell Sample::unitCell(int index)
{
    if (index<0 || static_cast<size_t>(index) >= _cells.size()) {
        throw std::runtime_error("Unit Cell not valid, asked for index " + std::to_string(index) + " of " + std::to_string(_cells.size()));
    }
    return _cells[index];
}

const UnitCellList& Sample::unitCells() const
{
    return _cells;
}

std::size_t Sample::nCrystals() const
{
    return _cells.size();
}

void Sample::removeUnitCell(sptrUnitCell cell)
{
    for (auto it = _cells.begin(); it != _cells.end(); ++it) {
        if ( *it == cell) {
            it = _cells.erase(it);
            break;
        }
    }
}

void Sample::removeUnitCell(int index)
{
    if (index < 0 || static_cast<size_t>(index) >= _cells.size()) {
        return;
    }
    _cells.erase(_cells.begin()+index);
}

unsigned int Sample::z(int index) const
{
    if (index < 0 || static_cast<size_t>(index) >= _cells.size()) {
        throw std::runtime_error("Invalid unit cell index.");
    }
    return _cells[index]->z();
}

void Sample::setZ(int Z, int index)
{
    if (index < 0 || static_cast<size_t>(index) >= _cells.size()) {
        throw std::runtime_error("Invalid unit cell index.");
    }

    if (Z==0) {
        throw std::runtime_error("Invalid Z value.");
    }
    _cells[index]->setZ(Z);
}

sptrMaterial Sample::material(int index) const
{
    if (index < 0 || static_cast<size_t>(index) >= _cells.size()) {
        throw std::runtime_error("Invalid unit cell index.");
    }
    return _cells[index]->material();
}

void Sample::setMaterial(sptrMaterial material, int index)
{
    if (index < 0 || static_cast<size_t>(index) >= _cells.size()) {
        throw std::runtime_error("Invalid unit cell index.");
    }
    _cells[index]->setMaterial(material);
}

} // end namespace nsx

