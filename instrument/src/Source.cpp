#include "Source.h"
#include "SourceFactory.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

Source* Source::create(const proptree::ptree& node)
{
	// Create an instance of the source factory
	SourceFactory* sourceFactory=SourceFactory::Instance();

	// Get the source type
	std::string sourceType=node.get<std::string>("<xmlattr>.type");

	// Fetch the source from the factory
	Source* source = sourceFactory->create(sourceType,node);

	return source;
}

Source::Source()
: Component("source"),
  _offset(0.0),
  _offsetFixed(true),
  _width(0.01),
  _height(0.01)
{
}

Source::Source(const Source& other)
: Component(other),
  _offset(other._offset),
  _offsetFixed(other._offsetFixed),
  _width(other._width),
  _height(other._height)
{
}

Source::Source(const std::string& name)
: Component(name),
  _offset(0.0),
  _offsetFixed(true),
  _width(0.01),
  _height(0.01)
{
}

Source::Source(const proptree::ptree& node)
: Component(node),
  _offset(0.0),
  _offsetFixed(true)
{

	Units::UnitsManager* um=SX::Units::UnitsManager::Instance();

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

Source::~Source()
{
}

Source& Source::operator=(const Source& other)
{
	if (this != &other)
	{
		Component::operator=(other);
		_offset = other._offset;
		_offsetFixed = other._offsetFixed;
		_width = other._width;
		_height = other._height;
	}
	return *this;
}

void Source::setOffset(double off)
{
	if (!_offsetFixed)
		_offset=off;
}
void Source::setOffsetFixed(bool fixed)
{
	_offsetFixed=fixed;
}

bool Source::hasOffsetFixed() const
{
	return _offsetFixed;
}

double Source::getWidth() const
{
	return _width;
}

void Source::setWidth(double width)
{
	_width=width;
}

double Source::getHeight() const
{
	return _height;
}

void Source::setHeight(double height)
{
	_height=height;
}

} // end namespace Instrument

} // end namespace SX
