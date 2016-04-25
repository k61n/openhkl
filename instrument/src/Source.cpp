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
  _selectedMonochromator(other._selectedMonochromator)
{
	_monochromators.reserve(other._monochromators.size());
	for (auto p : other._monochromators)
		_monochromators.push_back(new Monochromator(*p));
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
	    	Monochromator* mono = new Monochromator(v.second);
	    	addMonochromator(mono);
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
		_monochromators.reserve(other._monochromators.size());
		for (auto p : other._monochromators)
			_monochromators.push_back(new Monochromator(*p));
	}
	return *this;
}

void Source::setOffset(double offset) const
{
	Monochromator* mono=getSelectedMonochromator();
	if (mono == nullptr)
		return;

	if (mono->isOffsetFixed())
		return;

	mono->setOffset(offset);
}

void Source::setWavelength(double wavelength) const
{
	Monochromator* mono=getSelectedMonochromator();
	if (mono == nullptr)
		return;

	mono->setWavelength(wavelength);
}

double Source::getWavelength() const
{
	Monochromator* mono=getSelectedMonochromator();
	if (mono==nullptr)
		throw SX::Kernel::Error<Source>("No monochromator selected.");

	return mono->getWavelength();
}

double Source::getOffset() const
{
	Monochromator* mono=getSelectedMonochromator();
	if (mono==nullptr)
		throw SX::Kernel::Error<Source>("No monochromator selected.");

	return mono->getOffset();
}

void Source::setOffsetFixed(bool offsetFixed) const
{
	Monochromator* mono=getSelectedMonochromator();
	if (mono == nullptr)
		return;

	mono->setOffsetFixed(offsetFixed);
}

bool Source::isOffsetFixed() const
{
	Monochromator* mono=getSelectedMonochromator();
	if (mono==nullptr)
		throw SX::Kernel::Error<Source>("No monochromator selected.");

	return mono->isOffsetFixed();
}

const std::vector<Monochromator*>& Source::getMonochromators() const
{
	return _monochromators;
}

int Source::getNMonochromators() const
{
	return _monochromators.size();
}

Monochromator* Source::setSelectedMonochromator(size_t i)
{
	if (i >=0 && i<_monochromators.size())
	{
		_selectedMonochromator = i;
		return _monochromators[i];
	}
	else
		return nullptr;
}

Monochromator* Source::getSelectedMonochromator() const
{
	if (_selectedMonochromator>=0 && _selectedMonochromator<_monochromators.size())
		return _monochromators[_selectedMonochromator];
	else
		return nullptr;

}

void Source::addMonochromator(Monochromator* mono)
{

	auto it=std::find(_monochromators.begin(),_monochromators.end(),mono);
	if (it==_monochromators.end())
		_monochromators.push_back(mono);
}

Eigen::Vector3d Source::getKi() const
{
	Monochromator* mono=getSelectedMonochromator();
	if (mono==nullptr)
		throw SX::Kernel::Error<Source>("No monochromator selected.");

	return Eigen::Vector3d(0,1.0/mono->getWavelength(),0.0);
}

} // end namespace Instrument

} // end namespace SX
