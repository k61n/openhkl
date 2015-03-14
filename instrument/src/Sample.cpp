#include "Sample.h"

namespace SX
{

namespace Instrument
{

Sample::Sample() : Component("sample"), _sampleShape(), _material(),_cells()
{
}

Sample::Sample(const Sample& other) : Component(other), _sampleShape(other._sampleShape), _material(other._material)
{
}

Sample::Sample(const std::string& name) : Component(name), _sampleShape(), _material()
{
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
		_material=other._material;
	}
	return *this;
}

Component* Sample::clone() const
{
	return new Sample(*this);
}

SX::Geometry::ConvexHull<double>& Sample::getShape()
{
	return _sampleShape;
}

SX::Chemistry::sptrMaterial Sample::getMaterial() const
{
	return _material;
}

std::shared_ptr<SX::Crystal::UnitCell> Sample::addUnitCell()
{
	_cells.push_back(std::shared_ptr<SX::Crystal::UnitCell>(new SX::Crystal::UnitCell()));
	return (_cells.back());
}

std::shared_ptr<SX::Crystal::UnitCell> Sample::getUnitCell(int i)
{
	if (i>=_cells.size())
		throw std::runtime_error("Unit Cell not valid");
	return (_cells[i]);
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

} // end namespace Instrument

} /* namespace SX */
