#include <algorithm>
#include <vector>
#include "Error.h"
#include "SpaceGroup.h"
#include "SpaceGroupSymbols.h"

#include <boost/algorithm/string.hpp>

namespace SX
{

namespace Crystal
{


SpaceGroup::SpaceGroup(const std::string& symbol)
{
	// Get the generators of the input space group.
	SpaceGroupSymbols* sg=SpaceGroupSymbols::Instance();

	// Get a reduced version of the spacegroup symbol
	_symbol = sg->getReducedSymbol(symbol);

	if (!sg->getGenerators(_symbol,_generators))
        throw SX::Kernel::Error<SpaceGroup>("Unknown space group: " + _symbol + "(" + symbol + ")");
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

char SpaceGroup::getBravaisType() const
{
    std::vector<int> nrot(13,0);

    int nPureTrans(0);
    for (const auto& g : _groupElements)
    {
    	int order=g.getAxisOrder();
    	nrot[order+6] += 1;
    	if (g.isPureTranslation())
    		nPureTrans++;
    }

    int isCentro=isCentrosymmetric() ? 2 : 1;
    int fact = (1+nPureTrans)*isCentro;

    // Cubic
    if ((nrot[3] + nrot[9]) == 8*fact)
       return 'c';
    // Hexagonal
    else if ((nrot[0] + nrot[12]) == 2*fact)
       return 'h';
    // Trigonal
    else if ((nrot[3] + nrot[9]) == 2*fact)
        return 'h';
    // Tetragonal
    else if ((nrot[2] + nrot[10]) == 2*fact)
        return 't';
    // Orthorhombic
    else if ((nrot[4] + nrot[8]) == 3*fact)
        return 'o';
    // Monoclinic
    else if ((nrot[4] + nrot[8]) == fact)
        return 'm';
    // Triclinic
    else
        return 'a';
}

double SpaceGroup::fractionExtinct(std::vector<std::array<double, 3> > hkl)
{
    unsigned int extinct = 0;
    unsigned int total = hkl.size();

    for (auto& i: hkl) {
        if (isExtinct(i[0],i[1], i[2]))
            ++extinct;
    }

    return (double)extinct / (double)total;
}

std::string SpaceGroup::getBravaisTypeSymbol() const
{
    std::string bravais = "";
    bravais += getBravaisType();
    bravais += getSymbol()[0];
    return bravais;
}

int SpaceGroup::getID() const
{
    SpaceGroupSymbols* sg = SpaceGroupSymbols::Instance();
    std::string full_symbol = sg->getFullSymbol(_symbol);
    return sg->getID(full_symbol);
}

bool SpaceGroup::isCentrosymmetric() const
{
	for (const auto& g : _groupElements)
	{
		if (g.getAxisOrder()==-1)
			return true;
	}
	return false;
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

	generators.reserve(gens.size()+1);
	generators.push_back(SymOp(affineTransformation::Identity()));

	for (auto& g : gens)
	{
		auto gen=SymOp(g);
		generators.push_back(gen);
	}

	_groupElements.push_back(SymOp(affineTransformation::Identity()));

	while (true)
	{
		unsigned int oldSize=_groupElements.size();
		for (unsigned int i=0;i<_groupElements.size();++i)
		{
			for (const auto& g : generators)
			{
				auto newElement=_groupElements[i]*g;
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

bool SpaceGroup::isExtinct(double h, double k, double l) const
{
	Eigen::Vector3d hkl(h,k,l);
	for (const auto& element : _groupElements)
	{
		if (element.hasTranslation())
		{
			Eigen::Vector3d t=element.getTranslationPart();
			double scalar=t.dot(hkl);
			if (std::abs(std::remainder(scalar,1.0))>1e-3)
			{
				Eigen::Vector3d rhkl=element.getRotationPart()*hkl;
				if (std::abs(rhkl(0)-hkl(0))<1e-3 && std::abs(rhkl(1)-hkl(1))<1e-3 && std::abs(rhkl(2)-hkl(2))<1e-3)
				{
				return true;
				}
			}
		}
	}

	return false;
}

void SpaceGroup::print(std::ostream& os) const
{
	os << "Symmetry elements of space group "<<_symbol<<std::endl;
	for (unsigned int i=0;i<_groupElements.size();++i)
		os <<_groupElements[i]<< " ; ";
	os << std::endl;
}

std::ostream& operator<<(std::ostream& os, const SpaceGroup& sg)
{
	sg.print(os);
	return os;
}

} // end namespace Crystal

} // end namespace SX
