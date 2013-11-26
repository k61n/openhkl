#include "NumorSet.h"
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

namespace SX
{

namespace Data
{

NumorSet::NumorSet(const char* name):_name(name)
{
	// TODO Auto-generated constructor stub
}

NumorSet::~NumorSet()
{
	// TODO Auto-generated destructor stub
}

void NumorSet::addNumor(const sptrNumor& clone)
{
	// Add element in the map only if not exist
	int no=clone->getNo();
	numMapconstit it=_numors.find(no);
	if (it==_numors.end())
		_numors[no]=clone;
}

void NumorSet::addNumorFromFile(const std::string& filename)
{
	if ( !boost::filesystem::exists(filename.c_str() ) )
		throw std::runtime_error("Can't find file"+filename);
	// Make sure there is no problem lading the numor

	sptrNumor a(new SX::Numor(filename));
	// Insert in the map using the key/
	_numors[a->getNo()]=a;
}

void NumorSet::deleteNumor(int numor)
{
		_numors.erase(numor);
}

std::size_t NumorSet::getnElements() const
{
	return _numors.size();
}

std::vector<int> NumorSet::getNumorKeys() const
{
	std::vector<int> numbers;
	numbers.reserve(_numors.size());
	numMapconstit it=_numors.begin();
	for (;it!=_numors.end();++it)
	{
		numbers.push_back(it->first);
	}
	return numbers;
}

sptrNumor NumorSet::getNumor(int numor)
{
	numMapit it=_numors.find(numor);
	if (it!=_numors.end())
		return it->second;
	// Else return null
	return sptrNumor();
}

std::vector<sptrNumor> NumorSet::getNumors()
{
	std::vector<sptrNumor> temp;
	// Copy all elements in a vector
	std::transform(_numors.begin(),_numors.end(),std::back_inserter(temp),boost::bind(&numMap::value_type::second,_1));
	return temp;
}


} // end namespace Data

} // end namespace SX
