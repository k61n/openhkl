//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/PeakCollection.h
//! @brief     The peak collections within the core
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_SHAPE_PEAKCOLLECTION_H
#define NSX_CORE_SHAPE_PEAKCOLLECTION_H

#include "core/raw/MetaData.h"
#include "core/shape/ShapeCollection.h"

namespace nsx {

enum class listtype { FOUND, FILTERED, PREDICTED, INDEXING };

//! Container for a collection of peaks and its metadata
class PeakCollection {
 public:
    //! Default contructor
    PeakCollection();
    PeakCollection(const std::string& name, listtype type);

    //! Sets name of the PeakCollection
    void setName(const std::string& name);
    //! Returns the name of the PeakCollection
    std::string name() const;

    //! Populate the PeakCollection with a vector of shared pointers to peaks
    void populate(const std::vector<std::shared_ptr<nsx::Peak3D>> peak_list);
    //! Populate the PeakCollection with a vector of raw pointers to peaks
    void populate(const std::vector<nsx::Peak3D*> peak_list);
    //! Append one peak to the PeakCollection
    void push_back(const nsx::Peak3D& peak);
    //! Append one peak to the PeakCollection
    void addPeak(const std::shared_ptr<nsx::Peak3D>& peak);
    //! Populate from another collection, taking only peaks caughtByFilter
    void populateFromFiltered(PeakCollection* collection);
    //! Remove all peaks from the PeakCollection
    void reset();
    //! Return the peak with the given index
    const nsx::Peak3D* getPeak(int index) const { return _peaks.at(index).get(); }
    //! Return the peak with the given index
    nsx::Peak3D* getPeak(int index) { return _peaks.at(index).get(); }

    //! Return a std::vector of pointers to peaks
    std::vector<nsx::Peak3D*> getPeakList() const;
    //! Return a std::vector of pointers to peaks caughtByFilter
    std::vector<nsx::Peak3D*> getFilteredPeakList() const;

    //! Return the listtype of the PeakCollection (FOUND, PREDICTED, etc.)
    nsx::listtype type() const { return _type; };
    //! Set the listtype of the PeakCollection (FOUND, PREDICTED, etc.)
    void setType(listtype type) { _type = type; };

    //! compute beam divergence and mosaicity sigmas
    void computeSigmas();
    //! Return beam divergence sigma
    double sigmaD() const;
    //! Return mosaicity sigma
    double sigmaM() const;

    //! Set Miller indices of peaks for those with an assigned unit cell
    void setMillerIndices() const;

    //! Get the number of peaks int he PeakCollection
    int numberOfPeaks() const { return _peaks.size(); }
    //! Return the number of valid peaks
    int numberOfValid() const;
    //! Return the number of invalid peaks
    int numberOfInvalid() const;
    // Return number of peaks caught by filters
    int numberCaughtByFilter() const;
    // Return number of peaks rejected by filters
    int numberRejectedByFilter() const;

    //! Return a fresh generated pointer to metadata
    MetaData& metadata();

    //! Set the shape collection needed for profile integration
    void setShapeCollection(std::unique_ptr<ShapeCollection>& shape_collection);
    //! Clear the shape collection
    void resetShapeCollection() { _shape_collection.reset(nullptr); };
    //! Get the shape collection
    ShapeCollection* shapeCollection() const { return _shape_collection.get(); };

    void printUnitCells() const;

    //! Check the collection for unphysical  peaks
    void checkCollection() const;

    //! Count selected peaks
    int countSelected() const;
    //! Count enabled peaks
    int countEnabled() const;

 private:
    std::vector<std::unique_ptr<nsx::Peak3D>> _peaks;
    std::string _name;

    nsx::listtype _type{listtype::FOUND};
    nsx::MetaData _metadata;

    std::string _file_name;
    std::string _parent;

    std::unique_ptr<ShapeCollection> _shape_collection;

    //! Beam divergence sigma
    double _sigma_d;
    //! Mosaicity sigma
    double _sigma_m;
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_PEAKCOLLECTION_H
