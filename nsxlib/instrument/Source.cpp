#include <boost/foreach.hpp>

#include "../kernel/Error.h"
#include "Monochromator.h"
#include "Source.h"
#include "../utils/Units.h"

namespace nsx
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
            addMonochromator(m);
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
    if (i<_monochromators.size())
        _selectedMonochromator = i;
    else
        throw std::runtime_error("setSelectedMonochromator(): index i is out of range");
}

Monochromator& Source::getSelectedMonochromator()
{
    if (_selectedMonochromator<_monochromators.size())
        return _monochromators[_selectedMonochromator];
    else
        throw std::runtime_error("getSelectedMonochromator(): selected monochromator does not exist");
}

void Source::addMonochromator(Monochromator mono)
{
    _monochromators.push_back(mono);
}

} // end namespace Instrument

} // end namespace nsx
