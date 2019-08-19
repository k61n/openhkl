//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/PeakCollection.cpp
//! @brief     The peak collections within the core
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/PeakCollection.h"

namespace nsx {

PeakCollection::PeakCollection(std::string name, nsx::listtype type){
    _name = name;
    _type = type;
}

void PeakCollection::populate(
    const std::vector<std::shared_ptr<nsx::Peak3D>>* peak_list) 
{
    reset();

    for (std::shared_ptr<nsx::Peak3D> peak : *peak_list){
        std::unique_ptr<nsx::Peak3D> ptr(new Peak3D(peak));
        _peaks.push_back(std::move(ptr));
    }
}

void PeakCollection::reset()
{
    _peaks.clear();
}

void PeakCollection::addPeak(const std::shared_ptr<nsx::Peak3D> peak)
{
    std::unique_ptr<nsx::Peak3D> ptr(new Peak3D(peak));
    _peaks.push_back(std::move(ptr));
}

nsx::Peak3D* PeakCollection::getPeak(int index){
    return _peaks.at(index).get();
}

std::vector<nsx::Peak3D*>* PeakCollection::getPeakList()
{
    std::vector<nsx::Peak3D*>* peak_list = new std::vector<nsx::Peak3D*>;
    for (int i = 0; i<_peaks.size();i++) {
        peak_list->push_back(_peaks[i].get());
    }
    return peak_list;
}


int PeakCollection::numberOfPeaks() const 
{
    return _peaks.size();
}

int PeakCollection::numberOfValid() const 
{
    int valid = 0;
    for (int i = 0; i<_peaks.size();i++) {
        if (_peaks.at(i)->enabled())
            valid++;
    }
    return valid;
}

int PeakCollection::numberOfInvalid() const 
{
    return numberOfPeaks()-numberOfValid();
}


} // namespace nsx