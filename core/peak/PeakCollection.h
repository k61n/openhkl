//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/PeakCollection.h
//! @brief     The peak collections within the core
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_PEAKCOLLECTION_H
#define CORE_PEAK_PEAKCOLLECTION_H

#include "core/peak/Peak3D.h"

namespace nsx {

enum class listtype {FOUND,FILTERED,PREDICTED};

//! Container for a peaklist and its metadata
class PeakCollection{

 public:
    //!Default contructor
    PeakCollection(std::string name, nsx::listtype type);

    //! Returns the amount of invalid peaks
    std::string getName() const;

    //! Populate the peaks
    void populate(const std::vector<std::shared_ptr<nsx::Peak3D>>* peak_list);
    //! Add a single peak
    void addPeak(const std::shared_ptr<nsx::Peak3D> peak);
    //! Returns the peak with the given index
    nsx::Peak3D* getPeak(int index);    
    //! Returns the peak with index
    std::vector<nsx::Peak3D*>* getPeakList();

    //! Returns the amount of peaks
    int numberOfPeaks() const;
    //! Returns the amount of valid peaks
    int numberOfValid() const;
    //! Returns the amount of invalid peaks
    int numberOfInvalid() const;



private:
    std::vector<std::unique_ptr<nsx::Peak3D>> _peaks;
    std::string _name;

    nsx::listtype _type{listtype::FOUND};
    std::map<std::string, float> _meta;

    std::string _file_name;
    std::string _parent;
};

} // namespace nsx

#endif // GUI_MODELS_SESSIONEXPERIMENT_H