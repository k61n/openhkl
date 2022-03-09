//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/PeakHandler.h
//! @brief     Handles peak manipulations for Experiment object
//! //! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_PEAKHANDLER_H
#define NSX_CORE_EXPERIMENT_PEAKHANDLER_H

#include <map>
#include <vector>
#include <string>

#include "core/shape/PeakCollection.h"

namespace nsx {

using PeakCollectionMap = std::map<std::string, std::unique_ptr<PeakCollection>>;

class Peak3D;

using PeakList = std::vector<sptrPeak3D>;

class PeakHandler {

 public:
    PeakHandler() = default;
    ~PeakHandler();

 public: // handling peak collections
    //! Get a pointer to the map of peak collections
    const PeakCollectionMap* getPeakCollectionMap() const;
    //! Add a peak collection
    bool addPeakCollection(
        const std::string& name, const listtype type, const std::vector<nsx::Peak3D*> peaks);
    //
    bool addPeakCollection(
        const std::string& name,const listtype type, const std::vector<nsx::Peak3D*> peaks,
        bool indexed, bool integrated
        );
    //! Add an empty peak collection
    void addEmptyCollection(const std::string& name, const listtype type);
    //! Returns true if the experiment has named peak collection
    bool hasPeakCollection(const std::string& name) const;
    //! Returns the named peak collection
    PeakCollection* getPeakCollection(const std::string name);
    // !Remove a peak collection from the experiment
    void removePeakCollection(const std::string& name);
    //! Get a vector of peak collection names from the handler
    std::vector<std::string> getCollectionNames() const;
    //! Get a vector of peak collections with specific listtypes
    std::vector<std::string> getCollectionNames(listtype lt) const;
    //! Get the number of peak collections
    int numPeakCollections() const { return _peak_collections.size(); };
    //! Create a new collection of peaks caught by _peak_filter
    void acceptFilter(
        const std::string name, PeakCollection* collection, listtype lt = listtype::FILTERED);
    //! Deep copy a peak collection
    void clonePeakCollection(std::string name, std::string new_name);
    //! Generate name for new peak collection
    std::string GenerateName();

 private:
    //! Pointer to map of peak collections in Experiment
    PeakCollectionMap _peak_collections;
  
    //enum class listtype { FOUND, FILTERED, PREDICTED, INDEXING };
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_PEAKHANDLER_H
