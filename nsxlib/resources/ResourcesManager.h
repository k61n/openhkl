#pragma once

#include <map>
#include <string>

#include <yaml-cpp/yaml.h>

namespace nsx {

class ResourcesManager {
public:

    ResourcesManager(const std::string& resource_type);

    const YAML::Node& operator[](const std::string& resource_name) const;

private:

    std::string _resource_type;

    std::map<std::string,YAML::Node> _resources;
};

const ResourcesManager& databasesManager();

const ResourcesManager& instrumentsManager();

} // end namespace nsx
