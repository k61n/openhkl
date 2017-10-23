#include <stdexcept>

#include "IsotopeDatabaseManager.h"
#include "Path.h"
#include "Units.h"
#include "YAMLType.h"

namespace nsx {

std::map<std::string,ChemicalPropertyType> IsotopeDatabaseManager::PropertyTypes = {{"string",ChemicalPropertyType::String},
                                                                      {"int",ChemicalPropertyType::Int},
                                                                      {"double",ChemicalPropertyType::Double},
                                                                      {"complex",ChemicalPropertyType::Complex},
                                                                      {"bool",ChemicalPropertyType::Bool}};

IsotopeDatabaseManager::IsotopeDatabaseManager()
{
    std::string databaseFile = buildPath(applicationDataPath(),{"databases","isotopes.yml"});

    // No file existence checking, the YAML database is part of the distribution
    YAML::Node database = YAML::LoadFile(databaseFile);

    for (const auto& propertyNode : database["Properties"]) {
    	std::pair<std::string,std::string> prop = std::make_pair(propertyNode.second["type"].as<std::string>(),propertyNode.second["unit"].as<std::string>());
    	_properties.insert(std::make_pair(propertyNode.first.as<std::string>(),prop));
    }

    UnitsManager* um=UnitsManager::Instance();

    for (const auto& isotopeNode : database["Isotopes"]) {
    	isotopeProperties props;
        for (const auto& propertyNode : isotopeNode.second) {

        	std::string pname = propertyNode.first.as<std::string>();

            switch (PropertyTypes[_properties[pname].first]) {

            case ChemicalPropertyType::String:
                props.insert(std::make_pair(pname,propertyNode.second.as<std::string>()));
                break;
            case ChemicalPropertyType::Int:
                props.insert(std::make_pair(pname,propertyNode.second.as<int>()));
                break;
            case ChemicalPropertyType::Double:
                props.insert(std::make_pair(pname,propertyNode.second.as<double>()*um->get(_properties[pname].second)));
                break;
            case ChemicalPropertyType::Complex:
                props.insert(std::make_pair(pname,propertyNode.second.as<std::complex<double>>()*um->get(_properties[pname].second)));
                break;
            case ChemicalPropertyType::Bool:
                props.insert(std::make_pair(pname,propertyNode.second.as<bool>()));
                break;
            }
        }
        _isotopes.insert(std::make_pair(isotopeNode.first.as<std::string>(),props));
    }
}

const std::map<std::string,std::pair<std::string,std::string>>& IsotopeDatabaseManager::properties() const {
	return _properties;
}

const std::map<std::string,isotopeProperties>& IsotopeDatabaseManager::isotopes() const
{
    return _isotopes;
}

unsigned int IsotopeDatabaseManager::nIsotopes() const {
	return _isotopes.size();
}

bool IsotopeDatabaseManager::hasProperty(const std::string& isotope, const std::string& property) const
{
	auto iit = _isotopes.find(isotope);
	if (iit == _isotopes.end())
		return false;

    auto pit = iit->second.find(property);

    return (pit != iit->second.end());
}

} // end namespace nsx
