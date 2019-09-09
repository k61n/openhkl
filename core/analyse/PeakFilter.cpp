//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/analyse/PeakFilter.cpp
//! @brief     Implements class PeakFilter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <iterator>
#include <limits>
#include <map>
#include <set>

#include "base/geometry/ReciprocalVector.h"
#include "core/analyse/MergedData.h"
#include "core/analyse/Octree.h"
#include "core/analyse/PeakFilter.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

PeakFilter::PeakFilter()
{
    _filter_compute.set(0);
    _unit_cell = "";
    _unit_cell_tolerance = 0.2;
    _strength = {1.0,1000000.};
    _d_range = {1.5, 50.0};
    _significance = 0.9900;
    _sparse = 100;
}

void PeakFilter::filterSignificance(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    filterIndexTolerance(peak_collection);
    std::map<nsx::sptrUnitCell, std::vector<nsx::Peak3D*>> peaks_per_unit_cell;
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()){
            auto unit_cell = peak_ptr->unitCell();
            auto it = peaks_per_unit_cell.find(unit_cell);

            if (it == peaks_per_unit_cell.end()){
                std::vector<nsx::Peak3D*> temp {peak_ptr};
                peaks_per_unit_cell.insert(std::make_pair(unit_cell, temp));
            }else{
                it->second.push_back(peak_ptr);
            }
        }
    }

    for (auto p : peaks_per_unit_cell) {
        auto unit_cell = p.first;

        SpaceGroup group(unit_cell->spaceGroup());
        MergedData merged(group, true);
        PeakList filtered_peaks;

        for (auto peak : filtered_peaks)
            merged.addPeak(peak);

        for (auto&& merged_peak : merged.mergedPeakSet()) {
            if (merged_peak.pValue() > _significance) {
                for (auto&& p : merged_peak.peaks())
                    p->rejectYou(true);
            }
        }
    }

}

void PeakFilter::filterSparseDataSet(PeakCollection* peak_collection) const
{
    std::map<sptrDataSet, std::vector<nsx::Peak3D*>> peaks_per_dataset;

    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        auto data = peak_ptr->data();
        if (!data)
            continue;

        auto it = peaks_per_dataset.find(data);
        if (it == peaks_per_dataset.end()){
            std::vector<nsx::Peak3D*> temp {peak_ptr};
            peaks_per_dataset.insert(std::make_pair(data, temp));
        }else{
            it->second.push_back(peak_ptr);
        }
    }

    for (auto p : peaks_per_dataset) {
        if (p.second.size() > _sparse){
            for (auto peak : p.second)
                peak->caughtYou(true);
        }else{
            for (auto peak : p.second)
                peak->rejectYou(true);
        }
    }
}

void PeakFilter::filterExtincted(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    filterIndexTolerance(peak_collection);
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()){
            nsx::sptrUnitCell unit_cell = peak_ptr->unitCell();
            SpaceGroup group(unit_cell->spaceGroup());
            MillerIndex hkl(peak_ptr->q(), *(unit_cell));
            if (group.isExtinct(hkl)) {
                peak_ptr->rejectYou(true);
            }
        }
    }
}

void PeakFilter::filterOverlapping(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    std::vector<Ellipsoid> ellipsoids;
    std::set<Octree::collision_pair> collisions;
    Eigen::Vector3d lower(
        std::numeric_limits<double>::infinity(), 
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());
    Eigen::Vector3d upper(-lower);

    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        peak_ptr->caughtYou(true);
        auto&& ellipsoid = peak_ptr->shape();
        ellipsoids.emplace_back(ellipsoid);

        Eigen::Vector3d p = ellipsoid.center();

        for (int i = 0; i < 3; ++i) {
            lower(i) = std::min(lower(i), p(i));
            upper(i) = std::max(upper(i), p(i));
        }
    }

    // build octree
    Octree tree(lower, upper);

    for (unsigned int i = 0; i < peak_collection->numberOfPeaks(); ++i)
        tree.addData(&ellipsoids[i]);

    collisions = tree.getCollisions();
    
    // handle collisions below
    for (auto collision : collisions) {
        unsigned int i = collision.first - &ellipsoids[0];
        unsigned int j = collision.second - &ellipsoids[0];
        peak_collection->getPeak(i)->rejectYou(true);
        peak_collection->getPeak(j)->rejectYou(true);
    }
}

void PeakFilter::filterComplementary(PeakCollection* peak_collection) const
{
    // PeakList filtered_peaks;

    // for (auto peak : peaks) {
    //     auto it = std::find(other_peaks.begin(), other_peaks.end(), peak);
    //     if (it == other_peaks.end())
    //         filtered_peaks.push_back(peak);
    // }
}

void PeakFilter::filterEnabled(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->enabled()) {
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

std::vector<Peak3D*>* PeakFilter::filterEnabled(const std::vector<Peak3D*>* peaks_ptr, bool flag) const
{
  std::vector<Peak3D*>* filtered_peaks = new std::vector<Peak3D*>;
  std::vector<Peak3D*> peaks = *peaks_ptr;
  std::copy_if(peaks.begin(), peaks.end(), std::back_inserter(*filtered_peaks),
               [flag](Peak3D* peak) { return flag == peak->enabled(); });
  return filtered_peaks;
}

void PeakFilter::filterSelected(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->selected()) {
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filterMasked(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->masked()){ 
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filterIndexed(PeakCollection* peak_collection) const
{
    filterHasUnitCell(peak_collection);
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->caughtByFilter()){
            nsx::sptrUnitCell unit_cell = peak_ptr->unitCell();
            MillerIndex hkl(peak_ptr->q(), *unit_cell);
            if (hkl.indexed(_unit_cell_tolerance)) {
            peak_ptr->caughtYou(true);
            }else{
                peak_ptr->rejectYou(true);
            }
        }
    }
}

std::vector<Peak3D*>* PeakFilter::filterIndexed(
    const std::vector<Peak3D*>* peaks_ptr, const UnitCell &cell, double tolerance) const
{
std::vector<Peak3D*>* filtered_peaks = new std::vector<Peak3D*>;
  std::vector<Peak3D*> peaks = *peaks_ptr;
  for (auto peak : peaks) {
    MillerIndex miller_index(peak->q(), cell);
    if (miller_index.indexed(tolerance)) {
      filtered_peaks->push_back(peak);
    }
  }
  return filtered_peaks;
}

void PeakFilter::filterIndexTolerance(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        auto cell = peak_ptr->unitCell();
        if (!cell)
            continue;
        MillerIndex miller_index(peak_ptr->q(), *cell);
        if (miller_index.indexed(cell->indexingTolerance())) {
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filterUnitCell(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->unitCell()->name() == _unit_cell) {
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filterStrength(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        auto corrected_intensity = peak_ptr->correctedIntensity();
        double intensity = corrected_intensity.value();
        double sigma = corrected_intensity.sigma();

        if (sigma < 1.0e-6){
            peak_ptr->rejectYou(true);
            continue;
        }

        double i_over_sigma = intensity / sigma;
        if (i_over_sigma >= _strength[0] && i_over_sigma <= _strength[1]) {
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filterPredicted(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        if (peak_ptr->predicted()){
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filterDRange(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        auto q = peak_ptr->q();
        double d = 1.0 / q.rowVector().norm();
        if ((d >= _d_range[0] && d <= _d_range[1])){
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filterHasUnitCell(PeakCollection* peak_collection) const
{
    nsx::Peak3D* peak_ptr;
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_ptr = peak_collection->getPeak(i);
        auto cell = peak_ptr->unitCell();
        if (cell != nullptr) {
            peak_ptr->caughtYou(true);
        }else{
            peak_ptr->rejectYou(true);
        }
    }
}

void PeakFilter::filter(PeakCollection* peak_collection) const
{
    if (_filter_compute[7]){
        if (_filter_compute[0])
            filterSelected(peak_collection);
        if (_filter_compute[1])
            filterMasked(peak_collection);
        if (_filter_compute[2])
            filterPredicted(peak_collection);
    }

    if (_filter_compute[3])
        filterIndexed(peak_collection);

    if (_filter_compute[8]) {
        if (!(_unit_cell == "")) {
            filterIndexTolerance(peak_collection);
        }
    }

    if (_filter_compute[9])
        filterStrength(peak_collection);

    if (_filter_compute[10])
        filterDRange(peak_collection);

    if (_filter_compute[4])
        filterExtincted(peak_collection);

    if (_filter_compute[11])
        filterSparseDataSet(peak_collection);

    if (_filter_compute[12])
        filterSignificance(peak_collection);

    if (_filter_compute[5])
        filterOverlapping(peak_collection);

    if (_filter_compute[6])
        filterComplementary(peak_collection);

}

void PeakFilter::resetFiltering(PeakCollection* peak_collection) const
{
    for (int i = 0 ; i < peak_collection->numberOfPeaks(); ++i){
        peak_collection->getPeak(i)->caughtYou(false);
        peak_collection->getPeak(i)->rejectYou(false);
    }
}

} // namespace nsx
