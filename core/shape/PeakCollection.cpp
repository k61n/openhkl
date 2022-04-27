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
#include "core/integration/ShapeIntegrator.h"
#include "core/raw/DataKeys.h"
#include <string>
#include <typeindex>

namespace nsx {

PeakCollection::PeakCollection()
    : _name{nsx::kw_peakCollectionDefaultName}
    , _type{nsx::listtype::FOUND}
    , _shape_collection(nullptr)
    , _indexed(false)
    , _integrated(false)
{
}

PeakCollection::PeakCollection(const std::string& name, nsx::listtype type)
    : _name{std::string(name)}
    , _type{type}
    , _shape_collection(nullptr)
    , _indexed(false)
    , _integrated(false)
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

MetaData& PeakCollection::metadata()
{
    _metadata.add<int>(nsx::at_peakCount, numberOfPeaks());
    _metadata.add<int>(nsx::at_peakType, static_cast<int>(type()));

    // converting booleans to std::strings with 1 bytes size
    // while saving data to files only Int32, String and DBL seemed to supported
    _metadata.add<std::string>(nsx::at_indexed, std::to_string(isIndexed()));
    _metadata.add<std::string>(nsx::at_integrated, std::to_string(isIntegrated()));
    return _metadata;
}

void PeakCollection::setShapeCollection(std::unique_ptr<ShapeCollection>& shape_collection)
{
    resetShapeCollection();
    _shape_collection = std::move(shape_collection);
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
    nsxlog(Level::Info, "PeakCollection: '" + _name + "'");
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

Peak3D* PeakCollection::findPeakByIndex(const MillerIndex& hkl)
{
    for (auto peak : getPeakList()) {
        const MillerIndex& peak_hkl = peak->hkl();
        if ((hkl.h() == peak_hkl.h()) && (hkl.k() == peak_hkl.k()) && (hkl.l() == peak_hkl.l())) {
            return peak;
        }
    }
    return nullptr;
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

void PeakCollection::buildShapeCollection(sptrDataSet data, const ShapeCollectionParameters& params)
{
    nsxlog(Level::Info, "PeakCollection::buildShapeCollection");
    _shape_collection =
        std::make_unique<ShapeCollection>(std::make_shared<ShapeCollectionParameters>(params));
    _shape_collection->parameters()->log(Level::Info);
    computeSigmas();
    _shape_collection->parameters()->sigma_d = _sigma_d;
    _shape_collection->parameters()->sigma_m = _sigma_m;

    std::set<sptrDataSet> datalist;
    datalist.insert(data);
    std::vector<Peak3D*> fit_peak_list = getPeakList();
    _shape_collection->integrate(fit_peak_list, datalist);

    // shape_collection.updateFit(1000); // This does nothing!! - zamaan
    nsxlog(Level::Info, "PeakCollection::buildShapeCollection finished");
}

void PeakCollection::setUnitCell(const sptrUnitCell& cell)
{
    for (auto* peak : getPeakList())
        peak->setUnitCell(cell);
}

} // namespace nsx
