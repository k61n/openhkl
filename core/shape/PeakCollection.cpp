//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/PeakCollection.cpp
//! @brief     The peak collections within the core
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/PeakCollection.h"
#include <QDebug>

namespace nsx {

PeakCollection::PeakCollection()
    : _name{"No Name"}
    , _type{nsx::listtype::FOUND}
{
}

PeakCollection::PeakCollection(const std::string& name, nsx::listtype type)
    : _name{std::string(name)}
    , _type{type}
{
}

void PeakCollection::populate(const std::vector<std::shared_ptr<nsx::Peak3D>> peak_list)
{
    reset();
    for (std::shared_ptr<nsx::Peak3D> peak : peak_list)
        push_back(*peak);
}

void PeakCollection::populate(const std::vector<nsx::Peak3D*> peak_list)
{
    reset();
    for (nsx::Peak3D* peak : peak_list)
        push_back(*peak);
}

void PeakCollection::push_back(const nsx::Peak3D& peak)
{
    _peaks.push_back(std::unique_ptr<nsx::Peak3D> {new Peak3D(peak)});
}

void PeakCollection::addPeak(const std::shared_ptr<nsx::Peak3D>& peak)
{
    push_back(peak);
}

void PeakCollection::populateFromFiltered(PeakCollection* collection)
{
    reset();

    std::vector<nsx::Peak3D*> peak_list = collection->getPeakList();

    for (nsx::Peak3D* peak : peak_list) {
        if (peak->caughtByFilter())
            push_back(*peak);
    }
}

void PeakCollection::reset()
{
    _peaks.clear();
}

nsx::Peak3D* PeakCollection::getPeak(int index)
{
    return _peaks.at(index).get();
}

std::vector<nsx::Peak3D*> PeakCollection::getPeakList() const
{
    std::vector<nsx::Peak3D*> peak_list;
    for (int i = 0; i < _peaks.size(); i++) {
        peak_list.push_back(_peaks[i].get());
    }
    return peak_list;
}

std::vector<nsx::Peak3D*> PeakCollection::getFilteredPeakList() const
{
    std::vector<nsx::Peak3D*> peak_list;
    for (int i = 0; i < _peaks.size(); i++) {
        if (_peaks[i]->caughtByFilter())
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
    for (int i = 0; i < _peaks.size(); i++) {
        if (_peaks.at(i)->enabled())
            valid++;
    }
    return valid;
}

int PeakCollection::numberOfInvalid() const
{
    return numberOfPeaks() - numberOfValid();
}

int PeakCollection::numberCaughtByFilter() const
{
    int caught = 0;
    for (int i = 0; i < _peaks.size(); ++i) {
        if (_peaks.at(i)->caughtByFilter()) {
            caught++;
        }
    }
    return caught;
}

int PeakCollection::numberRejectedByFilter() const
{
    return numberOfPeaks() - numberCaughtByFilter();
}

std::map<std::string, float>* PeakCollection::meta()
{
    _meta.clear();
    _meta.insert(std::make_pair(std::string("num_peaks"), numberOfPeaks()));
    _meta.insert(std::make_pair(std::string("num_valid"), numberOfValid()));
    _meta.insert(std::make_pair(std::string("num_invalid"), numberOfInvalid()));
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

void PeakCollection::printUnitCells()
{
    std::vector<nsx::Peak3D*> peak_list = getPeakList();
    int i = 1;
    for (auto peak : peak_list) {
        std::cout << i << " " << peak->unitCell()->toString() << std::endl;
        i++;
    }
}

void PeakCollection::checkCollection() const
{
    int n_nan = 0;
    int n_zero = 0;
    double epsilon = 1.0e-8;
    for (Peak3D* peak : getPeakList()) {
        try {
            double I = peak->correctedIntensity().value();
            if (std::fabs(I) < epsilon)
                ++n_zero;
        } catch (std::range_error& e) {
            ++n_nan;
        }
    }
    qDebug() << "Peak collection " << QString::fromStdString(_name)
             << " contains " << numberOfPeaks() << " peaks:";
    qDebug() << n_nan << " peaks with intensity NaN";
    qDebug() << n_zero << " peaks with intensity zero";
}

} // namespace nsx
