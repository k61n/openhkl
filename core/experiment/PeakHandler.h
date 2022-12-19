//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakHandler.h
//! @brief     Handles peak manipulations for Experiment object
//! //! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_PEAKHANDLER_H
#define OHKL_CORE_EXPERIMENT_PEAKHANDLER_H

#include <map>
#include <string>
#include <vector>

#include "core/data/DataSet.h"
#include "core/shape/PeakCollection.h"

namespace ohkl {

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
        const std::string& name, const PeakCollectionType type,
        const std::vector<ohkl::Peak3D*> peaks, sptrDataSet data);
    //
    bool addPeakCollection(
        const std::string& name, const PeakCollectionType type,
        const std::vector<ohkl::Peak3D*> peaks, sptrDataSet data,
        bool indexed, bool integrated, bool gradient);
    //! Add an empty peak collection
    bool addEmptyCollection(const std::string& name, const PeakCollectionType type);
    //! Returns true if the experiment has named peak collection
    bool hasPeakCollection(const std::string& name) const;
    //! Returns true if the exp has a peakcollection of a certain type
    bool hasPeakCollectionType(PeakCollectionType t) const;
    //! Returns whether or not peak collection as integrated peaks
    bool hasIntegratedPeakCollection();
    //! Returns the named peak collection
    PeakCollection* getPeakCollection(const std::string name);
    // !Remove a peak collection from the experiment
    void removePeakCollection(const std::string& name);
    //! Get a vector of peak collection names from the handler
    std::vector<std::string> getCollectionNames() const;
    //! Get a vector of peak collections with specific PeakCollectionTypes
    std::vector<std::string> getCollectionNames(PeakCollectionType pct) const;
    //! Get the number of peak collections
    int numPeakCollections() const { return _peak_collections.size(); };
    //! Create a new collection of peaks caught by _peak_filter
    bool acceptFilter(
        const std::string name, PeakCollection* collection,
        PeakCollectionType pct, sptrDataSet data);
    //! Deep copy a peak collection
    bool clonePeakCollection(std::string name, std::string new_name);
    //! Generate name for new peak collection
    std::string generateName();
    //! Get a vector of pointers to peak collections
    std::vector<PeakCollection*> getPeakCollections();
    //! Set the last index for naming peak collections
    void setLastIndex(unsigned int last_index);

 private:
    //! Pointer to map of peak collections in Experiment
    PeakCollectionMap _peak_collections;
    unsigned int _last_index = 0;

    // enum class PeakCollectionType { FOUND, PREDICTED, INDEXING };
};

} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_PEAKHANDLER_H
