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

/*! \addtogroup python_api
 *  @{*/

//! Type of peak collection
enum class listtype { FOUND, FILTERED, PREDICTED, INDEXING };

/*! \brief Store a collection of peak shapes,  for peak prediction and integration.
 *
 * Container for a collection of peaks and its metadata
 */

class PeakCollection {
 public:
    //! Default contructor
    PeakCollection();
    PeakCollection(const std::string& name, listtype type);
    PeakCollection(const std::string& name, listtype type, bool indexed, bool integrated);

    //! Sets name of the PeakCollection
    void setName(const std::string& name);
    //! Returns the name of the PeakCollection
    std::string name() const;
    //! Returns description of the PeakCollection
    std::string description() const {return _description;};
    //! set description
    void setDescription(std::string str) {_description=str;};

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
    //! Find the peak with the given MillerIndex
    Peak3D* findPeakByIndex(const MillerIndex& hkl);

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

    //! Count selected peaks
    int countSelected() const;
    //! Count enabled peaks
    int countEnabled() const;

    //! Build shape collection from strong peaks in this peak collection
    void buildShapeCollection(sptrDataSet data, const ShapeCollectionParameters& params);


    bool isIndexed() const { return _indexed;}
    bool isIntegrated() const {return _integrated;}

    void setIndexed(bool value) {_indexed = value;}
    void setIntegrated(bool value) {_integrated = value;}

 private:
    std::vector<std::unique_ptr<nsx::Peak3D>> _peaks;
    std::string _name;
    std::string _description;

    nsx::listtype _type{listtype::FOUND};
    nsx::MetaData _metadata;

    std::string _file_name;
    std::string _parent;

    std::unique_ptr<ShapeCollection> _shape_collection;

    //! Beam divergence sigma
    double _sigma_d;
    //! Mosaicity sigma
    double _sigma_m;

    //! has been indexed already 
    bool _indexed;

    //! has been integrated
    bool _integrated;
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_SHAPE_PEAKCOLLECTION_H
