#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Path.h"
#include "ResourcesFactory.h"
#include "ResourcesManager.h"

namespace nsx {

ResourcesManager::ResourcesManager(const std::string& resource_type)
: _resource_type(resource_type)
{
    auto resource_path = buildPath({applicationDataPath(),resource_type});
    makeDirectory(resource_path);

    ResourcesFactory resource_factory;

    auto& resources = resource_factory.create(resource_type);

    for (auto res : resources) {
        std::cout<<res.first<<std::endl;
//        auto path = buildPath({resource_path,res.first+".yml"});
//        std::ofstream fout(path.c_str());
//        fout << res.second;
//        fout.close();
    }
}

const YAML::Node& ResourcesManager::operator[](const std::string& resource_name) const
{
    auto it=_resources.find(resource_name);
    if (it == _resources.end()) {
        throw std::runtime_error("Unknown resources: "+resource_name);
    }

    return _resources.at(resource_name);
}

static ResourcesManager g_databases("databases");
static ResourcesManager g_instruments("instruments");

const ResourcesManager& databasesManager()
{
    return g_databases;
}

const ResourcesManager& instrumentsManager()
{
    return g_instruments;
}

} // end namespace nsx
