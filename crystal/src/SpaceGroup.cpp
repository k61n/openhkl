#include <algorithm>
#include <iostream>

#include "Error.h"
#include "SpaceGroup.h"
#include "SpaceGroupGenerator.h"
#include "SpaceGroupSymbols.h"

#include <boost/algorithm/string.hpp>

namespace SX
{

namespace Crystal
{


SpaceGroup::SpaceGroup(const std::string& symbol) : _symbol(symbol)
{
	// Get the generators of the input space group.
	SpaceGroupSymbols* sg=SpaceGroupSymbols::Instance();
	if (!sg->getGenerators(_symbol,_generators))
		throw SX::Kernel::Error<SpaceGroup>("Unknown space group.");
	generateGroupElements();
}

SpaceGroup::SpaceGroup(const SpaceGroup& other)
: _symbol(other._symbol),
  _generators(other._generators),
  _groupElements(other._groupElements)
{
}

SpaceGroup::SpaceGroup(const std::string& symbol, const std::string& generators)
: _symbol(symbol),
  _generators(generators)
{
	SpaceGroupSymbols* sg=SpaceGroupSymbols::Instance();
	if (sg->getGenerators(_symbol,_generators))
		throw SX::Kernel::Error<SpaceGroup>("Space Group already registered");

	sg->addSpaceGroup(_symbol,_generators);

	generateGroupElements();
}

SpaceGroup& SpaceGroup::operator=(const SpaceGroup& other)
{
	if (this!=&other)
	{
		_symbol = other._symbol;
		_generators = other._generators;
		_groupElements = other._groupElements;
	}
	return *this;
}

const std::string& SpaceGroup::getSymbol() const
{
	return _symbol;
}

const std::string& SpaceGroup::getGenerators() const
{
	return _generators;
}

const groupElementsList& SpaceGroup::getGroupElements() const
{
	return _groupElements;
}

void SpaceGroup::generateGroupElements()
{
	_groupElements.clear();

	groupElementsList generators;

	std::vector<std::string> gens;
	boost::split(gens, _generators, boost::is_any_of(";"));
	for (auto& g : gens)
	{
		auto gen=SpaceGroupGenerator(g);
		generators.push_back(gen);
		SpaceGroupGenerator geninv(gen.getSymmetryOperation().inverse());
		auto it=std::find(generators.begin(),generators.end(),geninv);
		if (it==generators.end())
			generators.push_back(geninv);
	}

	_groupElements.push_back(SpaceGroupGenerator(affineTransformation::Identity()));

	while (true)
	{
		int oldSize=_groupElements.size();
		for (const auto& g : generators)
		{
			for (const auto& element : _groupElements)
			{
				auto newElement=element*g;
				auto it=std::find(_groupElements.begin(),_groupElements.end(),newElement);
				if (it==_groupElements.end())
					_groupElements.push_back(newElement);
			}
		}
		if (_groupElements.size()==oldSize)
			break;
		oldSize=_groupElements.size();
	}
}

} // end namespace Crystal

} // end namespace SX
