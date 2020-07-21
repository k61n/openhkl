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

#include "core/shape/ShapeLibrary.h"

namespace nsx {

enum class listtype { FOUND, FILTERED, PREDICTED };

//! Container for a peaklist and its metadata
class PeakCollection {

 public:
    //! Default contructor
    PeakCollection();
    PeakCollection(const std::string& name, nsx::listtype type);

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
    nsx::Peak3D* getPeak(int index);

    //! Returns the peak with index
    std::vector<nsx::Peak3D*> getPeakList() const;
    //! Returns the peak with index
    std::vector<nsx::Peak3D*> getFilteredPeakList() const;

    //! Returns the peak with index
    nsx::listtype type() const { return _type; };
    //! Returns the peak with index
    void setType(listtype type) { _type = type; };

    //! Returns the amount of peaks
    int numberOfPeaks() const;
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

    //! Set the shape library needed for the peak prediction
    void setShapeLibrary(ShapeLibrary shape_library);
    //! Reset the shape library
    void resetShapeLibrary() { _shape_library.reset(nullptr); };
    //! Get the shape library
    ShapeLibrary* shapeLibrary() const { return _shape_library.get(); };

    void printUnitCells();

    //! Check the collection for unphysical  peaks
    void checkCollection() const;

 private:
    std::vector<std::unique_ptr<nsx::Peak3D>> _peaks;
    std::string _name;

    nsx::listtype _type {listtype::FOUND};
    std::map<std::string, float> _meta;

    std::string _file_name;
    std::string _parent;

    std::unique_ptr<ShapeLibrary> _shape_library;
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_PEAKCOLLECTION_H
