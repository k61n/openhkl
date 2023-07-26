//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakHandler.cpp
//! @brief     Handles peak manipulations for Experiment object
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/PeakHandler.h"
#include "base/utils/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"

#include <stdexcept>

namespace ohkl {

PeakHandler::~PeakHandler() = default;

const PeakCollectionMap* PeakHandler::getPeakCollectionMap() const
{
    return &_peak_collections;
}

bool PeakHandler::addPeakCollection(
    const std::string& name, const PeakCollectionType type, const std::vector<ohkl::Peak3D*> peaks,
    sptrDataSet data, sptrUnitCell cell)
{
    // abort if name is aleady in use
    if (hasPeakCollection(name))
        return false;
    ohklLog(Level::Info, "PeakHandler::addPeakCollection '", name, "': ", peaks.size(), " peaks");
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type, data));
    ptr->populate(peaks);
    ptr->setId(_last_index++);
    ptr->setUnitCell(cell, false);
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name); // now name must be in use
}

bool PeakHandler::addPeakCollection(
    const std::string& name, const PeakCollectionType type, const std::vector<ohkl::Peak3D*> peaks,
    sptrDataSet data, sptrUnitCell cell, bool indexed, bool integrated, bool gradient)
{
    // abort if name is aleady in use
    if (hasPeakCollection(name))
        return false;
    ohklLog(Level::Info, "PeakHandler::addPeakCollection '", name, "': ", peaks.size(), " peaks");
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type, data));
    ptr->setId(_last_index++);
    ptr->setUnitCell(cell, false);
    ptr->setIndexed(indexed);
    ptr->setIntegrated(integrated);
    ptr->setBkgGradient(gradient);
    ptr->populate(peaks);
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name); // now name must be in use
}

bool PeakHandler::addEmptyCollection(const std::string& name, const PeakCollectionType type)
{
    if (hasPeakCollection(name))
        return false;
    ohklLog(Level::Info, "PeakHandler::addEmptyCollection '" + name + "'");
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, type, nullptr));
    ptr->setId(_last_index++);
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name); // now name must be in use
}

bool PeakHandler::hasPeakCollection(const std::string& name) const
{
    auto peaks = _peak_collections.find(name);
    return (peaks != _peak_collections.end());
}

bool PeakHandler::hasPeakCollectionType(PeakCollectionType t) const
{
    for (auto& e : _peak_collections)
        if (e.second->type() == t)
            return true;
    return false;
}

bool PeakHandler::hasIntegratedPeakCollection()
{
    for (auto& e : _peak_collections)
        if (e.second->isIntegrated())
            return true;
    return false;
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
        --_last_index;
    }
}

bool PeakHandler::acceptFilter(
    std::string name, PeakCollection* collection, PeakCollectionType pct, sptrDataSet data)
{
    if (hasPeakCollection(name))
        return false;
    std::unique_ptr<PeakCollection> ptr(new PeakCollection(name, pct, data));
    ptr->populateFromFiltered(collection);
    _peak_collections.insert_or_assign(name, std::move(ptr));
    return hasPeakCollection(name);
}

bool PeakHandler::clonePeakCollection(std::string name, std::string new_name)
{
    if (name == new_name)
        return false;
    if (!addEmptyCollection(new_name, getPeakCollection(name)->type())) {
        return false;
    }
    auto* peaks = getPeakCollection(name);
    auto* new_peaks = getPeakCollection(new_name);
    new_peaks->setData(peaks->data());
    new_peaks->populate(peaks->getPeakList());
    new_peaks->setIndexed(peaks->isIndexed());
    new_peaks->setIntegrated(peaks->isIntegrated());
    new_peaks->setUnitCell(peaks->unitCell());
    new_peaks->setType(peaks->type());
    return hasPeakCollection(name);
}

std::string PeakHandler::generateName()
{
    int n = 4; // number of digits
    std::string str = std::to_string(_last_index);
    if (str.size() > n) { //
        return "Please enter name for this collection";
    }
    return std::string("PeakCollection") + std::string(n - str.size(), '0').append(str);
}

std::vector<PeakCollection*> PeakHandler::getPeakCollections()
{
    std::vector<PeakCollection*> collections;
    for (const auto& [name, ptr] : _peak_collections)
        collections.push_back(ptr.get());
    return collections;
}

std::vector<PeakCollection*> PeakHandler::getPeakCollections(sptrDataSet data)
{
    std::vector<PeakCollection*> collections;
    for (const auto& [name, ptr] : _peak_collections) {
        if (ptr->data() == data)
            collections.push_back(ptr.get());
    }
    return collections;
}

void PeakHandler::setLastIndex(unsigned int last_index)
{
    _last_index = last_index;
}

} // namespace ohkl
