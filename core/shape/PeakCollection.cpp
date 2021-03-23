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
#include "base/utils/Logger.h"

namespace nsx {

PeakCollection::PeakCollection() : _name{"No Name"}, _type{nsx::listtype::FOUND} { }

PeakCollection::PeakCollection(const std::string& name, nsx::listtype type)
    : _name{std::string(name)}, _type{type}
{
}

void PeakCollection::populate(const std::vector<std::shared_ptr<nsx::Peak3D>> peak_list)
{
    reset();
    for (const auto& peak : peak_list)
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
    _peaks.push_back(std::unique_ptr<nsx::Peak3D>{new Peak3D(peak)});
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

std::vector<nsx::Peak3D*> PeakCollection::getPeakList() const
{
    std::vector<nsx::Peak3D*> peak_list(_peaks.size());
    for (int i = 0; i < _peaks.size(); i++)
        peak_list[i] = _peaks[i].get();
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
        if (_peaks.at(i)->caughtByFilter())
            caught++;
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

void PeakCollection::setShapeCollection(ShapeCollection shape_collection)
{
    _shape_collection = std::make_unique<ShapeCollection>(shape_collection);
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

void PeakCollection::printUnitCells() const
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
    const double epsilon = 1.0e-8;
    const std::vector<nsx::Peak3D*>& peak_list = getPeakList();
    for (const auto* const peak : peak_list) {
        try {
            double I = peak->correctedIntensity().value();
            if (std::fabs(I) < epsilon)
                ++n_zero;
        } catch (std::range_error& e) {
            ++n_nan;
        }
    }
    nsxlog(Level::Info, "PeakCollection::checkCollection: peak collection", _name, "contains:");
    nsxlog(Level::Info, numberOfPeaks(), "peaks");
    nsxlog(Level::Info, n_nan, "peaks with intensity NaN");
    nsxlog(Level::Info, n_zero, "peaks with intensity zero");
}

void PeakCollection::computeSigmas()
{
    Eigen::Matrix3d cov;
    cov.setZero();
    int npeaks = 0;
    for (auto peak : getPeakList()) {
        if (peak->enabled()) {
            PeakCoordinateSystem coord{peak};
            Ellipsoid shape = peak->shape();
            Eigen::Matrix3d J = coord.jacobian();
            cov += J * shape.inverseMetric() * J.transpose();
            ++npeaks;
        }
    }
    cov /= npeaks;
    _sigma_d = std::sqrt(0.5 * (cov(0, 0) + cov(1, 1)));
    _sigma_m = std::sqrt(cov(2, 2));
    nsxlog(
        Level::Info, "PeakCollection::computeSigmas: Beam divergence sigma and mosaicity sigma:");
    nsxlog(Level::Info, "PeakCollection: ", _name);
    nsxlog(Level::Info, "sigma_d = ", _sigma_d);
    nsxlog(Level::Info, "sigma_m = ", _sigma_m);
}

double PeakCollection::sigmaD() const
{
    return _sigma_d;
}

double PeakCollection::sigmaM() const
{
    return _sigma_m;
}

void PeakCollection::setMillerIndices() const
{
    for (auto peak : getPeakList())
        peak->setMillerIndices();
}

int PeakCollection::countSelected() const
{
    int nselected = 0;
    for (auto peak : getPeakList()) {
        if (peak->selected())
            ++nselected;
    }
    return nselected;
}

int PeakCollection::countEnabled() const
{
    int nenabled = 0;
    for (auto peak : getPeakList()) {
        if (peak->enabled())
            ++nenabled;
    }
    return nenabled;
}

} // namespace nsx
