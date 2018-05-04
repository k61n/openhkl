#include "Databases.h"
#include "Instruments.h"
#include "ResourcesFactory.h"

#include <iostream>
namespace nsx {

ResourcesFactory::ResourcesFactory(): _callbacks()
{
    _callbacks["databases"] = &databasesResources;
    _callbacks["instruments"]  = &instrumentsResources;
}

const std::map<std::string,std::string>& ResourcesFactory::create(const std::string& resource_type) const
{
    const auto it = _callbacks.find(resource_type);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error(resource_type + " is not a registered resource");
    }

    return (it->second)();
}

const std::map<std::string,ResourcesFactory::callback>& ResourcesFactory::callbacks() const
{
    return _callbacks;
}

} // end namespace nsx
