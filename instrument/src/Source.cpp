#include <boost/foreach.hpp>

#include "Error.h"
#include "Monochromator.h"
#include "Source.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

Source* Source::create(const proptree::ptree& node)
{
	// Fetch the source from the factory
	Source* source = new Source(node);
	return source;
}

Source::Source()
: Component("source"),
  _monochromators(),
  _selectedMonochromator(0)
{
}

Source::Source(const Source& other)
: Component(other),
  _monochromators(other._monochromators),
  _selectedMonochromator(other._selectedMonochromator)
{
}

Source::Source(const std::string& name)
: Component(name),
  _monochromators(),
  _selectedMonochromator(0)
{
}

Source::Source(const proptree::ptree& node)
: Component(node),
  _selectedMonochromator(0)
{
    // Loop over the "monochromator" nodes and add the corresponding pointer to Monochromator objects to the Source
	BOOST_FOREACH(const boost::property_tree::ptree::value_type& v, node)
	{
	    if (v.first.compare("monochromator")==0)
	    {
            Monochromator m(v.second);
	    	addMonochromator(&m);
	    }
	}
}

Source* Source::clone() const
{
	return new Source(*this);
}

Source::~Source()
{
}

Source& Source::operator=(const Source& other)
{
	if (this != &other)
	{
		Component::operator=(other);
		_selectedMonochromator = other._selectedMonochromator;
		_monochromators = other._monochromators;
	}
	return *this;
}

void Source::setOffset(double offset)
{
    auto mono = getSelectedMonochromator();
    
	if (mono->isOffsetFixed())
		return;

	mono->setOffset(offset);
}

void Source::setWavelength(double wavelength)
{
    auto mono = getSelectedMonochromator();
	mono->setWavelength(wavelength);
}

double Source::getWavelength() const
{
    auto mono = getSelectedMonochromator();
	return mono->getWavelength();
}

double Source::getOffset() const
{
    auto mono = getSelectedMonochromator();
	return mono->getOffset();
}

void Source::setOffsetFixed(bool offsetFixed) 
{
    auto mono = getSelectedMonochromator();
	mono->setOffsetFixed(offsetFixed);
}

bool Source::isOffsetFixed() const
{
    auto mono = getSelectedMonochromator();
	return mono->isOffsetFixed();
}

const std::vector<Monochromator>& Source::getMonochromators() const
{
	return _monochromators;
}

int Source::getNMonochromators() const
{
	return _monochromators.size();
}

void Source::setSelectedMonochromator(size_t i)
{
	if (i >=0 && i<_monochromators.size())
		_selectedMonochromator = i;
	else
		throw std::runtime_error("setSelectedMonochromator(): index i is out of range");
}

Monochromator* Source::getSelectedMonochromator()
{
	if (_selectedMonochromator>=0 && _selectedMonochromator<_monochromators.size())
		return &_monochromators[_selectedMonochromator];
	else
        throw std::runtime_error("getSelectedMonochromator(): selected monochromator does not exist");
}

const Monochromator* Source::getSelectedMonochromator() const
{
	if (_selectedMonochromator>=0 && _selectedMonochromator<_monochromators.size())
		return &_monochromators[_selectedMonochromator];
	else
        throw std::runtime_error("getSelectedMonochromator(): selected monochromator does not exist");
}

void Source::addMonochromator(Monochromator* mono)
{
	auto it=std::find(_monochromators.begin(),_monochromators.end(),*mono);
	if (it==_monochromators.end())
		_monochromators.push_back(*mono);
}

Eigen::Vector3d Source::getKi() const
{
	const Monochromator* mono=getSelectedMonochromator();
	return Eigen::Vector3d(0,1.0/mono->getWavelength(),0.0);
}

} // end namespace Instrument

} // end namespace SX
