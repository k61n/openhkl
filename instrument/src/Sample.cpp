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

Sample::Sample() : Component("sample"), _sampleShape(), _cells()
{
}

Sample::Sample(const Sample& other) : Component(other), _sampleShape(other._sampleShape)
{
}

Sample::Sample(const std::string& name) : Component(name), _sampleShape()
{
}

void Sample::buildFromXML(const property_tree::ptree& node)
{
	// Set the sample name from the XML node
	std::string sampleName=node.get<std::string>("name");
	this->setName(sampleName);

    // Set the sample goniometer from the XML node

    const property_tree::ptree& goniometerNode=node.get_child("goniometer");
    std::string goniometerName=goniometerNode.get<std::string>("name");
    std::shared_ptr<Gonio> gonio(new Gonio(goniometerName));

    // Set the axis of the sample goniometer from the XML node
	BOOST_FOREACH(const property_tree::ptree::value_type& v, goniometerNode)
	{
	    if (v.first.compare("axis")==0)
	    {
	    	std::string axisType=v.second.get<std::string>("<xmlattr>.type");
	    	std::string axisName=v.second.get<std::string>("name");

	    	const property_tree::ptree& axisDirectionNode=v.second.get_child("direction");
	    	double nx=axisDirectionNode.get<double>("x");
	    	double ny=axisDirectionNode.get<double>("y");
	    	double nz=axisDirectionNode.get<double>("z");

	    	Eigen::Vector3d axisDir(nx,ny,nz);
	    	axisDir.normalize();

	    	bool physical=v.second.get<bool>("physical");

	    	double offset=v.second.get<double>("offset");

	    	// Case of a rotation axis
	    	if (axisType.compare("rotation")==0)
	    	{
	    		bool clockwise=v.second.get<bool>("clockwise");
	    		RotAxis::Direction sense=clockwise ? RotAxis::Direction::CW : RotAxis::Direction::CCW;
		        gonio->addRotation(axisName,axisDir,sense);
	    		gonio->getAxis(axisName)->setPhysical(physical);
	    		gonio->getAxis(axisName)->setOffset(offset);
	    	}
	    	// Case of a translation axis
	    	else if (axisType.compare("translation")==0)
	    	{
		        gonio->addTranslation(axisName,axisDir);
	    		gonio->getAxis(axisName)->setPhysical(physical);
	    		gonio->getAxis(axisName)->setOffset(offset);
	    	}
	    	else
				throw Kernel::Error<Sample>("Invalid axis type. Must be one of 'rotation' or 'translation'.");
        }
	}

    this->setGonio(gonio);

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

Component* Sample::clone() const
{
	return new Sample(*this);
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
