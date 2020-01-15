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
#include <fstream>

namespace nsx {

PeakCollection::PeakCollection(){
    _name = "No Name";
    _type = nsx::listtype::FOUND;
}

PeakCollection::PeakCollection(const std::string& name, nsx::listtype type){
    _name = std::string(name);
    _type = type;
}

void PeakCollection::populate(
    const std::vector<std::shared_ptr<nsx::Peak3D>> peak_list) 
{
    reset();

    for (std::shared_ptr<nsx::Peak3D> peak : peak_list){
        std::unique_ptr<nsx::Peak3D> ptr(new Peak3D(peak));
        _peaks.push_back(std::move(ptr));
    }
}

void PeakCollection::populate(
    const std::vector<nsx::Peak3D*> peak_list) 
{
    reset();

    for (nsx::Peak3D* peak : peak_list){
        std::unique_ptr<nsx::Peak3D> ptr(new Peak3D(*peak));
        _peaks.push_back(std::move(ptr));
    }
}

void PeakCollection::populateFromFiltered(PeakCollection* collection) 
{
    reset();

    std::vector<nsx::Peak3D*> peak_list = collection->getPeakList();

    for (nsx::Peak3D* peak : peak_list){
        if (peak->caughtByFilter()){
            std::unique_ptr<nsx::Peak3D> ptr(new Peak3D(*peak));
            _peaks.push_back(std::move(ptr));
        }
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

std::vector<nsx::Peak3D*> PeakCollection::getPeakList() const 
{
    std::vector<nsx::Peak3D*> peak_list;
    for (int i = 0; i<_peaks.size();i++) {
        peak_list.push_back(_peaks[i].get());
    }
    return peak_list;
}

std::vector<nsx::Peak3D*> PeakCollection::getFilteredPeakList() const 
{
    std::vector<nsx::Peak3D*> peak_list;
    for (int i = 0; i<_peaks.size();i++) {
        if(_peaks[i]->caughtByFilter())
            peak_list.push_back(_peaks[i].get());
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

std::map<std::string, float>* PeakCollection::meta() 
{
    _meta.clear();
    _meta.insert(std::make_pair(std::string("num_peaks"),numberOfPeaks()));
    _meta.insert(std::make_pair(std::string("num_valid"),numberOfValid()));
    _meta.insert(std::make_pair(std::string("num_invalid"),numberOfInvalid()));
    // _meta.insert(std::make_pair(std::string(),));
    return &_meta;
}

void PeakCollection::setShapeLibrary(ShapeLibrary shape_library)
{
    _shape_library = std::make_unique<ShapeLibrary>(shape_library);
}

void PeakCollection::setName(const std::string& name)
{
    if (name.empty())
        return;
    _name = std::string(name);
}

std::string PeakCollection::name() const
{
    return std::string(_name);
}

bool PeakCollection::exportToGnuplot(const char* filename, bool recip) const
{
    std::ofstream ofstr(filename);
    if(!ofstr)
        return false;

    if(recip)
    {
        ofstr << "set xlabel 'h (rlu)'\n";
        ofstr << "set ylabel 'k (rlu)'\n";
        ofstr << "set zlabel 'l (rlu)'\n";
    }
    else
    {
        ofstr << "set xlabel 'Pixel'\n";
        ofstr << "set ylabel 'Pixel'\n";
        ofstr << "set zlabel 'Frame'\n";
    }

    ofstr << "set xyplane 0\n";

    ofstr << "splot '-' u 1:2:3:(sqrt($4*$5)/1e4) with points pt 7 ps variable lc black notitle\n";

    for(const auto& peak : _peaks)
    {
        nsx::Ellipsoid elli = recip ? peak->qShape() : peak->shape();
        ofstr << elli.center()[0] << " " << elli.center()[1] << " " << elli.center()[2];
        ofstr << " " << peak->correctedIntensity().value() << " " << peak->correctedIntensity().sigma() << "\n";
    }

    ofstr << "e" << std::endl;
    return true;
}

} // namespace nsx