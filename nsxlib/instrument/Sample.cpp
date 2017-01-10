#include <memory>

#include <boost/foreach.hpp>

#include <Eigen/Dense>

#include "Error.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "Sample.h"
#include "TransAxis.h"

namespace SX
{

namespace Instrument
{

Sample* Sample::create(const proptree::ptree& node)
{
	return new Sample(node);
}

Sample::Sample() : Component("sample"), _sampleShape(), _cells()
{
}

Sample::Sample(const Sample& other) : Component(other), _sampleShape(other._sampleShape)
{
}

Sample::Sample(const std::string& name) : Component(name), _sampleShape()
{
}

Sample::Sample(const proptree::ptree& node) : Component(node)
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
	if (this != &other)
	{
		Component::operator=(other);
		_sampleShape=other._sampleShape;
	}
	return *this;
}

void Sample::setShape(const SX::Geometry::ConvexHull<double>& shape)
{
	_sampleShape = shape;
}

SX::Geometry::ConvexHull<double>& Sample::getShape()
{
	return _sampleShape;
}

std::shared_ptr<SX::Crystal::UnitCell> Sample::addUnitCell()
{
	_cells.push_back(std::shared_ptr<SX::Crystal::UnitCell>(new SX::Crystal::UnitCell()));
	return (_cells.back());
}

std::shared_ptr<SX::Crystal::UnitCell> Sample::getUnitCell(unsigned int i)
{
	if (i>=_cells.size())
		throw std::runtime_error("Unit Cell not valid");
	return (_cells[i]);
}

const CellList& Sample::getUnitCells() const
{
	return _cells;
}

std::size_t Sample::getNCrystals() const
{
	return _cells.size();
}

void Sample::removeUnitCell(std::shared_ptr<SX::Crystal::UnitCell> cell)
{
	for (auto it=_cells.begin();it!=_cells.end();++it)
	{
		if ((*it)==cell)
		{
			_cells.erase(it);
			break;
		}
	}
}

unsigned int Sample::getZ(unsigned int cellIndex) const
{
	if (cellIndex >= _cells.size())
		throw Kernel::Error<Sample>("Invalid unit cell index.");

	return _cells[cellIndex]->getZ();
}

void Sample::setZ(unsigned int Z, unsigned int cellIndex)
{
	if (cellIndex >= _cells.size())
		throw Kernel::Error<Sample>("Invalid unit cell index.");

	if (Z==0)
		throw Kernel::Error<Sample>("Invalid Z value.");

	_cells[cellIndex]->setZ(Z);
}

Chemistry::sptrMaterial Sample::getMaterial(unsigned int cellIndex) const
{
	if (cellIndex >= _cells.size())
		throw Kernel::Error<Sample>("Invalid unit cell index.");

	return _cells[cellIndex]->getMaterial();
}

void Sample::setMaterial(Chemistry::sptrMaterial material, unsigned int cellIndex)
{
	if (cellIndex >= _cells.size())
		throw Kernel::Error<Sample>("Invalid unit cell index.");

	_cells[cellIndex]->setMaterial(material);
}

} // end namespace Instrument

} /* namespace SX */
