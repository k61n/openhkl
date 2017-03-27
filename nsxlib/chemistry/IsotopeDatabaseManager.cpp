#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>

#include "IsotopeDatabaseManager.h"
#include "../utils/Path.h"

namespace SX {

namespace Chemistry {

using boost::filesystem::exists;
using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::xml_parser::write_xml;
using boost::property_tree::ptree;

using SX::Utils::Path;

std::string IsotopeDatabaseManager::DatabasePath = Path::getDataBasesPath("isotopes");

IsotopeDatabaseManager::IsotopeDatabaseManager()
{
    loadDatabase(DatabasePath);
}

Isotope IsotopeDatabaseManager::getIsotope(const std::string& name) const
{
    auto it=_database.find(name);
    if (it==_database.end())
        throw std::runtime_error("Chemical object "+name+" not found in the database");

    return it->second;
}

isotopeDatabase& IsotopeDatabaseManager::getDatabase()
{
    return _database;
}

const isotopeDatabase& IsotopeDatabaseManager::getDatabase() const
{
    return _database;
}

void IsotopeDatabaseManager::loadDatabase(const std::string& filename)
{
    // The given path does not exists, throws
    if (!exists(filename)) {
        return;
    }

    _database.clear();

    // Parse and load the whole Isotope database in memory
    ptree root;
    read_xml(filename,root);
    for (const auto& node : root.get_child("isotopes"))
    {
        if (node.first.compare("isotope")!=0) {
            continue;
        }

        Isotope chemObj(node.second);
        _database.insert(std::make_pair(chemObj.getName(),chemObj));
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

} // end namespace Chemistry

} // end namespace SX
