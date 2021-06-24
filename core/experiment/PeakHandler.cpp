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
#include "core/statistics/MergedData.h"

namespace nsx {

PeakHandler::~PeakHandler() = default;

const PeakCollectionMap* PeakHandler::getPeakCollectionMap() const
{
    return &_peak_collections;
}

void PeakHandler::addPeakCollection(
    const std::string& name, const listtype type, const std::vector<nsx::Peak3D*> peaks)
{
    nsxlog(Level::Info, "PeakHandler::addPeakCollection: ", name, ": ", peaks.size(), "peaks");
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type));
    ptr->populate(peaks);
    _peak_collections.insert_or_assign(name, std::move(ptr));
}

void PeakHandler::addEmptyCollection(const std::string& name, const listtype type)
{
    nsxlog(Level::Info, "PeakHandler::addEmptyCollection: ", name);
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type));
    _peak_collections.insert_or_assign(name, std::move(ptr));
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

void PeakHandler::acceptFilter(std::string name, PeakCollection* collection, listtype lt)
{
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, lt));
    ptr->populateFromFiltered(collection);
    _peak_collections.insert_or_assign(name, std::move(ptr));
}

void PeakHandler::setMergedPeaks(std::vector<PeakCollection*> peak_collections, bool friedel)
{
    _merged_peaks = std::make_unique<MergedData>(peak_collections, friedel);
}

void PeakHandler::setMergedPeaks(PeakCollection* found, PeakCollection* predicted, bool friedel)
{
    std::vector<PeakCollection*> collections;
    collections.push_back(found);
    collections.push_back(predicted);
    _merged_peaks = std::make_unique<MergedData>(collections, friedel);
}

void PeakHandler::resetMergedPeaks()
{
    _merged_peaks.reset();
}

void PeakHandler::checkPeakCollections()
{
    for (const auto& [name, collection] : _peak_collections) {
        collection->checkCollection();
    }
}

void PeakHandler::clonePeakCollection(std::string name, std::string new_name)
{
    addEmptyCollection(new_name, getPeakCollection(name)->type());
    getPeakCollection(new_name)->populate(getPeakCollection(name)->getPeakList());
}

} // namespace nsx
