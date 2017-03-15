/*
 * ChemicalDatabaseManager.h
 *
 *  Created on: Mar 15, 2017
 *      Author: pellegrini
 */

#ifndef NSXLIB_CHEMISTRY_CHEMICALDATABASEMANAGER_H_
#define NSXLIB_CHEMISTRY_CHEMICALDATABASEMANAGER_H_

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "../kernel/Singleton.h"

namespace SX {

namespace Chemistry {

using boost::filesystem::exists;
using boost::property_tree::xml_parser::read_xml;
using boost::property_tree::ptree;

using SX::Kernel::Constructor;
using SX::Kernel::Destructor;
using SX::Kernel::Singleton;

template <typename ChemicalObject>
class ChemicalDatabaseManager : public Singleton<ChemicalDatabaseManager<ChemicalObject>,Constructor,Destructor>
{
public:

    using sptrChemicalObject = std::shared_ptr<ChemicalObject>;
    using ChemicalObjectDatabase=std::map<std::string,sptrChemicalObject>;

    ChemicalDatabaseManager();
    ~ChemicalDatabaseManager();

    sptrChemicalObject getChemicalObject(const std::string& name) const;

    const ChemicalObjectDatabase& getDatabase() const;

    ChemicalObjectDatabase& getDatabase();

    void loadDatabase(const std::string& filename);

private:

    ChemicalObjectDatabase _database;

};

template <typename ChemicalObject>
ChemicalDatabaseManager<ChemicalObject>::ChemicalDatabaseManager()
{
    loadDatabase(ChemicalObject::DatabasePath);
}

template <typename ChemicalObject>
ChemicalDatabaseManager<ChemicalObject>::~ChemicalDatabaseManager()
{
}

template <typename ChemicalObject>
typename ChemicalDatabaseManager<ChemicalObject>::sptrChemicalObject ChemicalDatabaseManager<ChemicalObject>::getChemicalObject(const std::string& name) const
{
    // If the chemical has already been registered, just return its corresponding pointer
    auto it=_database.find(name);
    if (it==_database.end())
        throw std::runtime_error("Chemical object "+name+" not found in the database");

    return it->second;
}

template <typename ChemicalObject>
typename ChemicalDatabaseManager<ChemicalObject>::ChemicalObjectDatabase& ChemicalDatabaseManager<ChemicalObject>::getDatabase()
{
    return _database;
}

template <typename ChemicalObject>
const typename ChemicalDatabaseManager<ChemicalObject>::ChemicalObjectDatabase& ChemicalDatabaseManager<ChemicalObject>::getDatabase() const
{
    return _database;
}

template <typename ChemicalObject>
void ChemicalDatabaseManager<ChemicalObject>::loadDatabase(const std::string& filename)
{
    std::cout<<filename<<std::endl;

    // The given path does not exists, throws
    if (!exists(filename)) {
        return;
    }

    std::cout<<filename<<std::endl;

    _database.clear();

    // Parse and load the whole Isotope database in memory
    ptree root;
    read_xml(filename,root);
    for (const auto& node : root.get_child(ChemicalObject::DatabaseRootNode))
    {
        sptrChemicalObject chemObj(new ChemicalObject(node.second));
        _database.insert(std::make_pair(chemObj->getName(),chemObj));
        std::cout<<node.first<<" --> "<<chemObj->getName()<<" "<<_database.size()<<std::endl;
    }
}

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXLIB_CHEMISTRY_CHEMICALDATABASEMANAGER_H_ */
