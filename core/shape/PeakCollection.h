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

#include "core/shape/ShapeCollection.h"

namespace nsx {

enum class listtype { FOUND, FILTERED, PREDICTED, INDEXING };

//! Container for a peaklist and its metadata
class PeakCollection {
 public:
    //! Default contructor
    PeakCollection();
    PeakCollection(const std::string& name, listtype type);

    //! Sets name of the unit cell
    void setName(const std::string& name);
    //! Returns the name
    std::string name() const;

    //! Populate the peaks
    void populate(const std::vector<std::shared_ptr<nsx::Peak3D>> peak_list);
    //! Populate the peaks
    void populate(const std::vector<nsx::Peak3D*> peak_list);
    //! Append one peak
    void push_back(const nsx::Peak3D& peak);
    //! Append one peak
    void addPeak(const std::shared_ptr<nsx::Peak3D>& peak);
    //! Populate the peaks
    void populateFromFiltered(PeakCollection* collection);
    //! Reset the peaks
    void reset();
    //! Returns the peak with the given index
    const nsx::Peak3D* getPeak(int index) const { return _peaks.at(index).get(); }
    nsx::Peak3D* getPeak(int index) { return _peaks.at(index).get(); }

    //! Returns the peak with index
    std::vector<nsx::Peak3D*> getPeakList() const;
    //! Returns the peak with index
    std::vector<nsx::Peak3D*> getFilteredPeakList() const;

    //! Returns the peak with index
    nsx::listtype type() const { return _type; };
    //! Returns the peak with index
    void setType(listtype type) { _type = type; };

    //! compute beam divergence and mosaicity sigmas
    void computeSigmas();
    //! Return beam divergence sigma
    double sigmaD();
    //! Return mosaicity sigma
    double sigmaM();

    //! Set Miller indices of peaks for those with an assigned unit cell
    void setMillerIndices() const;

    //! Returns the amount of peaks
    int numberOfPeaks() const { return _peaks.size(); }
    //! Returns the amount of valid peaks
    int numberOfValid() const;
    //! Returns the amount of invalid peaks
    int numberOfInvalid() const;
    // Returns number of peaks caught by filter
    int numberCaughtByFilter() const;
    // Returns number of peaks rejected by filter
    int numberRejectedByFilter() const;

    //! Returns a fresh generated pointer to meta
    std::map<std::string, float>* meta();

    //! Set the shape collection needed for the peak prediction
    void setShapeCollection(ShapeCollection shape_collection);
    //! Reset the shape collection
    void resetShapeCollection() { _shape_collection.reset(nullptr); };
    //! Get the shape collection
    ShapeCollection* shapeCollection() const { return _shape_collection.get(); };

    void printUnitCells();

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
    std::map<std::string, float> _meta;

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
