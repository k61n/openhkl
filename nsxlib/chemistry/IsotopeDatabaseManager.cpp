#include <stdexcept>
#include <iostream>

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

using boost::filesystem::exists;
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
    loadDatabase(DatabasePath);
}

const IsotopeDatabaseManager::Isotope& IsotopeDatabaseManager::getIsotope(const std::string& name) const
{
    auto it=_database.find(name);
    if (it==_database.end())
        throw std::runtime_error("Chemical object "+name+" not found in the database");

    return it->second;
}

const IsotopeDatabaseManager::isotopeDatabase& IsotopeDatabaseManager::database() const
{
    return _database;
}

void IsotopeDatabaseManager::loadDatabase(const std::string& filename)
{
    // The given path does not exists, throws
    if (!exists(filename)) {
        throw std::runtime_error("Unknown isotope database: "+filename);
    }

    _database.clear();

    YAML::Node database = YAML::LoadFile(filename);

    for (const auto& node : database) {
        Isotope chemObj(node.second);
        _database.insert(std::make_pair(node.first.as<std::string>(),chemObj));
    }
}

void IsotopeDatabaseManager::saveDatabase(std::string filename) const
{
    ptree root;
    ptree& rootNode = root.add("isotopes","");

    for (const auto& chemObject : _database)
    {
        auto chemObjectNode = chemObject.second.writeToXML();
        rootNode.add_child("isotope",chemObjectNode);
    }

    if (filename.empty())
        filename=DatabasePath;

#if BOOST_MINOR <= 55
    boost::property_tree::xml_writer_settings<char> settings('\t', 1);
#else
    auto settings = boost::property_tree::xml_writer_make_settings<std::string>('\t', 1);
#endif
    write_xml(filename,root,std::locale(),settings);

}

IsotopeDatabaseManager::Isotope::Isotope(const YAML::Node& isotopeNode)
{
    UnitsManager* um=UnitsManager::Instance();

    _name=isotopeNode["name"].as<std::string>();
    std::cout<<_name<<std::endl;

    for (const auto& propertyNode : isotopeNode) {

        std::string propertyName = propertyNode.first.as<std::string>();
        std::cout<<propertyName<<std::endl;

        const auto& pnode = propertyNode.second;
        _types[propertyName] = pnode["type"].as<std::string>();
        _units[propertyName] = pnode["units"].as<std::string>();

        std::cout<<_types[propertyName]<<" "<<_types[propertyName]<<std::endl;

        switch (PropertyTypes[_types[propertyName]]) {

        case PropertyType::String:
            _properties[propertyName] = pnode["value"].as<std::string>();
            break;
        case PropertyType::Int:
            _properties[propertyName] = pnode["value"].as<int>();
            break;
        case PropertyType::Double:
            _properties[propertyName] = pnode["value"].as<double>()*um->get(_units[propertyName]);
            break;
        case PropertyType::Complex:
            _properties[propertyName] = pnode["value"].as<std::complex<double>>()*um->get(_units[propertyName]);
            break;
        case PropertyType::Bool:
            _properties[propertyName] = pnode["value"].as<bool>();
            break;
        default:
            throw std::runtime_error("unknown property type for "+propertyName+" property");
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

ptree IsotopeDatabaseManager::Isotope::writeToXML() const
{
//    UnitsManager* um=UnitsManager::Instance();
//
    ptree node;
//    node.put("<xmlattr>.name",_name);
//    for (const auto& prop : _properties) {
//        std::string pname = prop.first;
//        ptree& isnode=node.add(pname,"");
//        isnode.put("<xmlattr>.type",_types.at(pname));
//        isnode.put("<xmlattr>.unit",_units.at(pname));
//
//        switch (PropertyTypes[_types.at(pname)]) {
//
//        case PropertyType::String:
//            isnode.put_value(any_cast<std::string>(prop.second));
//            break;
//        case PropertyType::Int:
//            isnode.put_value(any_cast<int>(prop.second));
//            break;
//        case PropertyType::Double:
//            isnode.put_value(any_cast<double>(prop.second)/um->get(_units.at(pname)));
//            break;
//        case PropertyType::Complex:
//            isnode.put_value(any_cast<std::complex<double>>(prop.second)/um->get(_units.at(pname)));
//            break;
//        case PropertyType::Bool:
//            isnode.put_value(any_cast<bool>(prop.second));
//            break;
//        }
//    }
    return node;
}

std::ostream& operator<<(std::ostream& os,const IsotopeDatabaseManager::Isotope& isotope)
{
    isotope.print(os);
    return os;
}

} // end namespace Chemistry

} // end namespace SX
