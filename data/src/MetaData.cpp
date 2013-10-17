
#include "MetaData.h"

namespace SX{


// Static that contains all available keys.
keyset MetaData::_metakeys=std::set<std::string>();

MetaData::MetaData()
{
}
MetaData::~MetaData()
{
}
MetaData::MetaData(const MetaData& rhs)
{
	_map=rhs._map;
}

MetaData& MetaData::operator=(const MetaData& rhs)
{
	if (this!=&rhs)
		_map=rhs._map;
	return *this;
}

const keyset& MetaData::getAllKeys() const
{
	return _metakeys;
}
bool MetaData::isKey(const char* key) const
{
	// Search if this key is in the set.
	keysetcit it=_metakeys.find(std::string(key));
	if (it==_metakeys.end())
		return false;
	const char* ptr=(*it).c_str();
	heterit it2=_map.find(ptr);
	return  (it2!=_map.end());
}
bool MetaData::isKey(const std::string& key) const
{
	// Search if this key is in the set.
	keysetcit it=_metakeys.find(key);
		if (it==_metakeys.end())
			return false;
		const char* ptr=(*it).c_str();
		heterit it2=_map.find(ptr);
		return  (it2!=_map.end());
}

boost::any MetaData::getKey(const std::string& key) const
{
	// Search if this key is in the set.
	keysetcit it=_metakeys.find(key);
		if (it==_metakeys.end())
			throw std::runtime_error("Could not locate key "+key+ " in the set of available keys");
		const char* ptr=(*it).c_str();
		heterit it2=_map.find(ptr);
		if (it2!=_map.end())
		{
			return it2->second;
		}
		else
			throw std::runtime_error("Could not locate key "+key+ " in the set of available keys");
}


} // End namespace SX.
