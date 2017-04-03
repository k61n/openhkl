#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>

#include "IsotopeDatabaseManager.h"
#include "../utils/Path.h"
#include "../utils/Units.h"
#include "../utils/YAMPType.h"

namespace SX {

namespace Chemistry {

using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::xml_parser::write_xml;
using boost::property_tree::ptree;

using SX::Utils::Path;
using SX::Units::UnitsManager;

std::string IsotopeDatabaseManager::DatabasePath = Path::getDataBasesPath("isotopes.yaml");

std::map<std::string,IsotopeDatabaseManager::PropertyType> IsotopeDatabaseManager::PropertyTypes = {{"string",PropertyType::String},
                                                                      {"int",PropertyType::Int},
                                                                      {"double",PropertyType::Double},
                                                                      {"complex",PropertyType::Complex},
                                                                      {"bool",PropertyType::Bool}};

IsotopeDatabaseManager::IsotopeDatabaseManager()
{
    // No file existence checking, the YAML database is part of the distribution
    YAML::Node allIsotopeNodes = YAML::LoadFile(DatabasePath);
    for (const auto& isotopeNode : allIsotopeNodes) {
        _database.insert(std::make_pair(isotopeNode.first.as<std::string>(),Isotope(isotopeNode.second)));
    }
}

const IsotopeDatabaseManager::Isotope& IsotopeDatabaseManager::getIsotope(const std::string& name) const
{
    auto it=_database.find(name);
    if (it == _database.end()) {
        throw std::runtime_error("Isotope "+name+" not found in the database");
    }
    return it->second;
}

const IsotopeDatabaseManager::isotopeDatabase& IsotopeDatabaseManager::database() const
{
    return _database;
}

IsotopeDatabaseManager::Isotope::Isotope(const YAML::Node& isotopeNode)
{
    UnitsManager* um=UnitsManager::Instance();

    for (const auto& propertyNode : isotopeNode) {

        std::string pname = propertyNode.first.as<std::string>();
        const auto& pnode = propertyNode.second;

        if (pname.compare("name")==0) {
            _name=pnode.as<std::string>();
            continue;
        }

        _types[pname] = pnode["type"].as<std::string>();
        _units[pname] = pnode["unit"].as<std::string>();

        switch (PropertyTypes[_types[pname]]) {

        case PropertyType::String:
            _properties[pname] = pnode["value"].as<std::string>();
            break;
        case PropertyType::Int:
            _properties[pname] = pnode["value"].as<int>();
            break;
        case PropertyType::Double:
            _properties[pname] = pnode["value"].as<double>()*um->get(_units[pname]);
            break;
        case PropertyType::Complex:
            _properties[pname] = pnode["value"].as<std::complex<double>>()*um->get(_units[pname]);
            break;
        case PropertyType::Bool:
            _properties[pname] = pnode["value"].as<bool>();
            break;
        default:
            throw std::runtime_error("unknown property type for "+pname+" property");
        }
    }
}

const std::string& IsotopeDatabaseManager::Isotope::getName() const
{
    return _name;
}

bool IsotopeDatabaseManager::Isotope::hasProperty(const std::string& propertyName) const
{
    auto it = _properties.find(propertyName);
    return (it != _properties.end());
}

void IsotopeDatabaseManager::Isotope::print(std::ostream& os) const
{
    os<<"Isotope "<<_name<<" ["<<getProperty<int>("n_protons")<<","<<getProperty<int>("n_neutrons")<<"]";
}

std::ostream& operator<<(std::ostream& os,const IsotopeDatabaseManager::Isotope& isotope)
{
    isotope.print(os);
    return os;
}

} // end namespace Chemistry

} // end namespace SX
