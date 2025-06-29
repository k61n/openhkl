//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/PeakCollection.h
//! @brief     The peak collections within the core
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_PEAKCOLLECTION_H
#define OHKL_CORE_SHAPE_PEAKCOLLECTION_H

#include "core/data/DataTypes.h"
#include "core/peak/Peak3D.h"
#include "core/raw/MetaData.h"

namespace ohkl {

enum class IntegratorType;
class MillerIndex;

/*! \addtogroup python_api
 *  @{*/

//! Type of peak collection
enum class PeakCollectionType { FOUND, PREDICTED, INDEXING };

/*! \brief Store a collection of peak shapes,  for peak prediction and integration.
 *
 * Container for a collection of peaks and its metadata
 */

class PeakCollection {
 public:
    //! Default contructor
    PeakCollection();
    PeakCollection(const std::string& name, PeakCollectionType type, sptrDataSet data);

    //! Get integer id
    unsigned int id() { return _id; };
    //! Set integer id
    void setId(unsigned int id);

    //! Sets name of the PeakCollection
    void setName(const std::string& name);
    //! Returns the name of the PeakCollection
    std::string name() const;
    //! Set the DataSet associated with these peaks
    void setData(sptrDataSet data) { _data = data; };
    //! Get the DataSet associated with these peaks
    sptrDataSet data() const { return _data; };
    //! Get the unit cell from autoindexing or used to predict these peaks
    sptrUnitCell unitCell() const { return _cell; };
    //! Returns description of the PeakCollection
    std::string description() const { return _description; };
    //! set description
    void setDescription(std::string str) { _description = str; };

    //! Populate the PeakCollection with a vector of shared pointers to peaks
    void populate(const std::vector<std::shared_ptr<ohkl::Peak3D>> peak_list);
    //! Populate the PeakCollection with a vector of raw pointers to peaks
    void populate(const std::vector<ohkl::Peak3D*> peak_list);
    //! Append one peak to the PeakCollection
    void push_back(const ohkl::Peak3D& peak);
    //! Append one peak to the PeakCollection
    void addPeak(const std::shared_ptr<ohkl::Peak3D>& peak);
    //! Populate from another collection, taking only peaks caughtByFilter
    void populateFromFiltered(PeakCollection* collection);
    //! Remove all peaks from the PeakCollection
    void reset();
    //! Return the peak with the given index
    const ohkl::Peak3D* getPeak(int index) const { return _peaks.at(index).get(); }
    //! Return the peak with the given index
    ohkl::Peak3D* getPeak(int index) { return _peaks.at(index).get(); }

    //! Return a std::vector of pointers to peaks
    std::vector<ohkl::Peak3D*> getPeakList() const;
    //! Return a std::vector of pointers to peaks caughtByFilter
    std::vector<ohkl::Peak3D*> getFilteredPeakList() const;

    //! Return the PeakCollectionType of the PeakCollection (FOUND, PREDICTED, etc.)
    ohkl::PeakCollectionType type() const { return _type; };
    //! Set the PeakCollectionType of the PeakCollection (FOUND, PREDICTED, etc.)
    void setType(PeakCollectionType type) { _type = type; };

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
    //! Find symmetry-related peaks for each peak
    void getSymmetryRelated(bool friedel = true);

    //! Get the number of peaks int he PeakCollection
    int numberOfPeaks() const { return _peaks.size(); }
    //! Return the number of valid peaks
    int numberOfValid() const;
    //! Return the number of invalid peaks
    int numberOfInvalid() const;
    //! Return number of peaks caught by filters
    int numberCaughtByFilter() const;
    //! Return number of peaks rejected by filters
    int numberRejectedByFilter() const;
    //! Return most frequent rejection flag
    RejectionFlag mostFrequentRejection() const;


    //! Return a fresh generated pointer to metadata
    MetaData& metadata();

    //! Count enabled peaks
    int countEnabled() const;

    //! Whether PeakCollection has been indexed or not
    bool isIndexed() const { return _indexed; }
    //! Whether PeakCollection has been integrated or not
    bool isIntegrated() const { return _integrated; }
    //! Whether PeakCollection has background gradient
    bool hasBkgGradient() const { return _gradient; }
    //! Set Indexed flag
    void setIndexed(bool value) { _indexed = value; }
    //! Count Integrated flag
    void setIntegrated(bool value) { _integrated = value; }
    //! Background gradient flag
    void setBkgGradient(bool value) { _gradient = value; }
    //! Assign a unit cell (only assign to peaks if setPeaks == true)
    void setUnitCell(const sptrUnitCell& cell, bool setPeaks = true);

    //! Reset integration status
    void resetIntegration(IntegratorType integrator_type);
    //! Reset peak rejection flags
    void resetRejectionFlags();
    //! Reset peak rejection status to pre-integration values
    void resetIntegrationFlags(IntegratorType integrator);

 private:
    unsigned int _id;
    std::vector<std::unique_ptr<ohkl::Peak3D>> _peaks;
    std::string _name;
    sptrDataSet _data;
    std::string _description;

    ohkl::PeakCollectionType _type{PeakCollectionType::FOUND};
    ohkl::MetaData _metadata;

    std::string _file_name;
    std::string _parent;

    //! Unit cell i) assigned by indexer ii) used to predict peaks (note that after
    //! refinement, different peaks may have different unit cell pointers)
    sptrUnitCell _cell;

    //! Beam divergence sigma
    double _sigma_d;
    //! Mosaicity sigma
    double _sigma_m;

    //! Has been indexed already
    bool _indexed;

    //! Has been integrated
    bool _integrated;

    //! Integration includes background gradient
    bool _gradient;

    //! Peaks have symmetry-equivalents populated
    bool _have_equivalents;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_PEAKCOLLECTION_H
