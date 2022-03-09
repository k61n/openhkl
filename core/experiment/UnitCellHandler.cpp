//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/UnitCellHandler.cpp
//! @brief     Handles unit cells manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/UnitCellHandler.h"
#include "base/utils/Logger.h"
#include "core/algo/AutoIndexer.h"
#include "core/experiment/PeakHandler.h"
#include "core/raw/DataKeys.h"
#include "core/shape/PeakCollection.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

const CellMap* UnitCellHandler::getCellMap() const
{
    return &_unit_cells;
}

bool UnitCellHandler::addUnitCell(const std::string& name, const UnitCell& unit_cell, bool refined)
{
    if (hasUnitCell(name)) return false;
    nsxlog(Level::Info, "UnitCellHandler::addUnitCell: '", name, "': ", unit_cell.toString());
    sptrUnitCell uc = std::make_shared<UnitCell>(unit_cell);
    uc->setName(name);
    if (refined)
        _batch_cells.insert_or_assign(name, std::move(uc));
    else
        _unit_cells.insert_or_assign(name, std::move(uc));
    return hasUnitCell(name); 
}

bool UnitCellHandler::addUnitCell(const std::string& name, sptrUnitCell unit_cell, bool refined)
{
    if (hasUnitCell(name)) return false;
    nsxlog(Level::Info, "UnitCellHandler::addUnitCell: '", name, "': ", unit_cell->toString());
    unit_cell->setName(name);
    if (refined)
        _batch_cells.insert_or_assign(name, std::move(unit_cell));
    else
        _unit_cells.insert_or_assign(name, std::move(unit_cell));
    return hasUnitCell(name); 
}

bool UnitCellHandler::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma)
{
    UnitCell cell{a, b, c, alpha * deg, beta * deg, gamma * deg};
    return addUnitCell(name, cell);
}

bool UnitCellHandler::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma,
    const std::string& space_group)
{
    UnitCell cell{a, b, c, alpha * deg, beta * deg, gamma * deg};
    cell.setSpaceGroup(space_group);
    return addUnitCell(name, cell);
}

bool UnitCellHandler::hasUnitCell(const std::string& name) const
{
    bool has = false;
    if (_unit_cells.find(name) != _unit_cells.end()) {
        has = true;
    } else {
        if (_batch_cells.find(name) != _batch_cells.end())
            has = true;
    }
    return has;
}

std::vector<std::string> UnitCellHandler::getUnitCellNames() const
{
    std::vector<std::string> names;
    for (CellMap::const_iterator it = _unit_cells.begin(); it != _unit_cells.end(); ++it) {
        names.push_back(it->second->name());
    }
    for (CellMap::const_iterator it = _batch_cells.begin(); it != _batch_cells.end(); ++it) {
        names.push_back(it->second->name());
    }
    return names;
}

sptrUnitCell UnitCellHandler::getSptrUnitCell(const std::string& name) const
{
    if (hasUnitCell(name)) {
        if (_unit_cells.find(name) != _unit_cells.end()) {
            auto it = _unit_cells.find(name);
            return it->second;
        } else {
            auto it = _batch_cells.find(name);
            return it->second;
        }
    }
    return nullptr;
}

UnitCell* UnitCellHandler::getUnitCell(const std::string& name) const
{
    return getSptrUnitCell(name).get();
}

void UnitCellHandler::removeUnitCell(const std::string& name)
{
    if (hasUnitCell(name)) {
        if (_unit_cells.find(name) != _unit_cells.end()) {
            auto unit_cell = _unit_cells.find(name);
            unit_cell->second.reset();
            _unit_cells.erase(unit_cell);
        } else {
            auto unit_cell = _batch_cells.find(name);
            unit_cell->second.reset();
            _batch_cells.erase(unit_cell);
        }
    }
}

void UnitCellHandler::swapUnitCells(
    const std::string& old_cell_name, const std::string& new_cell_name,
    PeakHandler* peak_handler) const
{
    sptrUnitCell old_cell = getSptrUnitCell(old_cell_name);
    sptrUnitCell new_cell = getSptrUnitCell(new_cell_name);
    const auto* peak_collections = peak_handler->getPeakCollectionMap();

    std::map<std::string, std::unique_ptr<PeakCollection>>::const_iterator it;
    for (it = peak_collections->begin(); it != peak_collections->end(); ++it) {
        std::vector<Peak3D*> peaks = it->second->getPeakList();
        for (Peak3D* peak : peaks) {
            if (peak->unitCell() == old_cell.get())
                peak->setUnitCell(new_cell);
        }
    }
}

void UnitCellHandler::setReferenceCell(
    double a, double b, double c, double alpha, double beta, double gamma)
{
    std::string name = nsx::kw_referenceUnitcell;
    UnitCell reference_cell{a, b, c, alpha * deg, beta * deg, gamma * deg};
    addUnitCell(name, reference_cell);
}

bool UnitCellHandler::checkAndAssignUnitCell(
    PeakCollection* peaks, AutoIndexer* auto_indexer, double length_tol, double angle_tol,
    std::string name)
{
    std::string ref_name = nsx::kw_referenceUnitcell;
    const UnitCell* ref_cell = getUnitCell(ref_name);
    bool accepted = false;
    sptrUnitCell good_cell = auto_indexer->goodSolution(ref_cell, length_tol, angle_tol);
    if (good_cell) {
        addUnitCell(name, *good_cell);
        assignUnitCell(peaks);
        accepted = true;
    }
    return accepted;
}

void UnitCellHandler::assignUnitCell(PeakCollection* peaks, std::string cellName) const
{
    std::vector<Peak3D*> peak_list = peaks->getPeakList();
    for (auto* peak : peak_list) {
        peak->setUnitCell(getSptrUnitCell(cellName));
        peak->setMillerIndices();
    }
}

std::vector<std::string> UnitCellHandler::getCompatibleSpaceGroups() const
{
    return getSptrUnitCell(nsx::kw_acceptedUnitcell)->compatibleSpaceGroups();
}

std::vector<sptrUnitCell> UnitCellHandler::extractBatchCells()
{
    std::vector<sptrUnitCell> vec;
    for (auto it = _batch_cells.cbegin(), next_it = it; it != _batch_cells.cend(); it = next_it) {
        ++next_it;
        vec.push_back(it->second);
        auto nh = _batch_cells.extract(it);
    }
    return vec;
}

void UnitCellHandler::mergeBatchCells(CellMap map)
{
    _unit_cells.merge(map);
}

std::string UnitCellHandler::GenerateUnitCellName()
{
    return std::string("UnitCellNr.:") + std::to_string(numUnitCells()+1);
}

} // namespace nsx
