#include <iostream>
#include <string>

#include <boost/foreach.hpp>

#include "Modifier.h"
#include "PrimitiveTransformation.h"
#include "PrimitiveTransformationFactory.h"

namespace SX
{

namespace Instrument
{

using SX::Geometry::PrimitiveTransformation;
using SX::Geometry::PrimitiveTransformationFactory;

Modifier::Modifier() : XMLConfigurable(), _trans()
{
}

Modifier::Modifier(const ptree& node)
{
	parse(node);
}

Modifier::~Modifier()
{
}

void Modifier::addTransformation(PrimitiveTransformation* trans)
{
	_trans.push_back(trans);
}

Matrix3d Modifier::getRotation(const std::vector<double>& parameters) const
{
	return (getTransformation(parameters).rotation());
}

HomMatrix Modifier::getTransformation(const std::vector<double>& parameters) const
{
	HomMatrix mat;
	mat.setIdentity();

	std::vector<PrimitiveTransformation*>::const_reverse_iterator it=_trans.rbegin();
	std::vector<double>::const_reverse_iterator pit=parameters.rbegin();

	for (;it!=_trans.rend();++it,++pit)
		mat.matrix() *= (*it)->getTransformation(*pit).matrix();

	return mat;
}

Vector3d Modifier::getTranslation(const std::vector<double>& parameters) const
{
	return (getTransformation(parameters).translation());
}

void Modifier::parse(const ptree& node)
{
	_name = node.get<std::string>("name");

	// Create (or call) an instance of the PrimitiveTransformation factory.
	PrimitiveTransformationFactory* factory = PrimitiveTransformationFactory::Instance();

	// Loop over the "component" nodes of the "multi_detector" XML node.
	BOOST_FOREACH(ptree::value_type v, node)
	{
		if (v.first == "transformation")
		{
			// Fetch the "type" attribute if the running transformation node and get the corresponding PrimitiveTransformation object from the factory.
			std::string tType=v.second.get<std::string>("<xmlattr>.type");
			std::cout<<"aaa"<<tType<<std::endl;
			PrimitiveTransformation* trans=factory->create(tType,v.second);
		}
	}

}

void Modifier::_parse(const ptree& node)
{
}

} // end namespace Instrument

} // end namespace SX
