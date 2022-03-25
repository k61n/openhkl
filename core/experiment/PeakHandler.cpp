//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/PeakHandler.cpp
//! @brief     Handles peak manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/PeakHandler.h"
#include "base/utils/Logger.h"
#include "core/peak/Peak3D.h"
#include <stdexcept>

namespace nsx {

PeakHandler::~PeakHandler() = default;

const PeakCollectionMap* PeakHandler::getPeakCollectionMap() const
{
    return &_peak_collections;
}

bool PeakHandler::addPeakCollection(
    const std::string& name, const listtype type, const std::vector<nsx::Peak3D*> peaks)
{
    // abort if name is aleady in use
    if (hasPeakCollection(name)) return false; 
    nsxlog(Level::Info, "PeakHandler::addPeakCollection '", name, "': ", peaks.size(), " peaks");
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type));
    ptr->populate(peaks);
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name); // now name must be in use
}

bool PeakHandler::addPeakCollection(
    const std::string& name, const listtype type, const std::vector<nsx::Peak3D*> peaks,
    bool indexed, bool integrated)
{
    // abort if name is aleady in use
    if (hasPeakCollection(name)) return false;
    nsxlog(Level::Info, "PeakHandler::addPeakCollection '", name, "': ", peaks.size(), " peaks");
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type));
    ptr->setIndexed(indexed);
    ptr->setIntegrated(integrated);
    ptr->populate(peaks);
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name); // now name must be in use
}

bool PeakHandler::addEmptyCollection(const std::string& name, const listtype type)
{
    if (hasPeakCollection(name)) return false;
    nsxlog(Level::Info, "PeakHandler::addEmptyCollection '" + name + "'");
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type));
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name); // now name must be in use
}

bool PeakHandler::hasPeakCollection(const std::string& name) const
{
    auto peaks = _peak_collections.find(name);
    return (peaks != _peak_collections.end());
}

PeakCollection* PeakHandler::getPeakCollection(const std::string name)
{
    if (hasPeakCollection(name)) {
        return _peak_collections[name].get();
    }
    return nullptr;
}

void PeakHandler::removePeakCollection(const std::string& name)
{
    if (hasPeakCollection(name)) {
        auto peak_collection = _peak_collections.find(name);
        peak_collection->second.reset();
        _peak_collections.erase(peak_collection);
    }
}

std::vector<std::string> PeakHandler::getCollectionNames() const
{

    std::vector<std::string> names;
    for (PeakCollectionMap::const_iterator it = _peak_collections.begin();
         it != _peak_collections.end(); ++it) {
        names.push_back(it->second->name());
    }
    return names;
}

std::vector<std::string> PeakHandler::getCollectionNames(
    listtype lt /* = listtype::FILTERED */) const
{
    std::vector<std::string> names;
    for (PeakCollectionMap::const_iterator it = _peak_collections.begin();
         it != _peak_collections.end(); ++it) {
        if (it->second->type() == lt)
            names.push_back(it->second->name());
    }
    return names;
}

bool PeakHandler::acceptFilter(std::string name, PeakCollection* collection, listtype lt)
{
    if (hasPeakCollection(name)) return false;
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, lt));
    ptr->populateFromFiltered(collection);
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name);
}

bool PeakHandler::clonePeakCollection(std::string name, std::string new_name)
{
    if (name == new_name) return false;
    if (!addEmptyCollection(new_name, getPeakCollection(name)->type())){
        return false;
    }
    getPeakCollection(new_name)->populate(getPeakCollection(name)->getPeakList());
    return hasPeakCollection(name);
}

std::string PeakHandler::GenerateName()
{
    int n = 4; // number of digits
    std::string str = std::to_string(numPeakCollections()+1);
    if (str.size() > n){//
        return "Please enter name for this collection";
    }
    return std::string("PeakCollectionNr") +  std::string( n - str.size(), '0').append( str );
}

} // namespace nsx
