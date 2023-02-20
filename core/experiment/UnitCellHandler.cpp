//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/UnitCellHandler.cpp
//! @brief     Handles unit cells manipulations for Experiment object
//!
//! @homepage  https://openhkl.org
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

namespace ohkl {

UnitCellHandler::UnitCellHandler()
{
    _unit_cells.reserve(_max_unit_cells);
}

const UnitCellList* UnitCellHandler::getCellList() const
{
    return &_unit_cells;
}

void UnitCellHandler::addUnitCell(const std::string& name, const UnitCell& unit_cell)
{
    ohklLog(Level::Info, "UnitCellHandler::addUnitCell: '", name, "': ", unit_cell.toString());
    sptrUnitCell uc = std::make_shared<UnitCell>(unit_cell);
    uc->setName(name);
    uc->setId(_last_index++);
    _unit_cells.push_back(std::move(uc));
}

void UnitCellHandler::addUnitCell(const std::string& name, sptrUnitCell unit_cell)
{
    ohklLog(Level::Info, "UnitCellHandler::addUnitCell: '", name, "': ", unit_cell->toString());
    unit_cell->setName(name);
    unit_cell->setId(_last_index++);
    _unit_cells.push_back(std::move(unit_cell));
}

void UnitCellHandler::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma)
{
    sptrUnitCell cell = std::make_shared<UnitCell>(a, b, c, alpha * deg, beta * deg, gamma * deg);
    cell->setName(name);
    cell->setId(_last_index++);
    _unit_cells.push_back(std::move(cell));
}

void UnitCellHandler::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma,
    const std::string& space_group)
{
    sptrUnitCell cell = std::make_shared<UnitCell>(a, b, c, alpha * deg, beta * deg, gamma * deg);
    cell->setSpaceGroup(space_group);
    cell->setName(name);
    cell->setId(_last_index++);
    _unit_cells.push_back(std::move(cell));
}

bool UnitCellHandler::hasUnitCell(const std::string& name) const
{
    for (const auto& cell : _unit_cells) {
        if (cell->name() == name)
            return true;
    }
    return false;
}

std::vector<std::string> UnitCellHandler::getUnitCellNames() const
{
    std::vector<std::string> names;
    for (const auto& cell : _unit_cells)
        names.push_back(cell->name());
    return names;
}

sptrUnitCell UnitCellHandler::getSptrUnitCell(const std::string& name) const
{
    for (const auto& cell : _unit_cells)
        if (cell->name() == name)
            return cell;
    return nullptr;
}

sptrUnitCell UnitCellHandler::getSptrUnitCell(const unsigned int id) const
{
    for (const auto& cell : _unit_cells)
        if (cell->id() == id)
            return cell;
    return nullptr;
}

UnitCell* UnitCellHandler::getUnitCell(const std::string& name) const
{
    if (hasUnitCell(name))
        return getSptrUnitCell(name).get();
    return nullptr;
}

void UnitCellHandler::removeUnitCell(const std::string& name)
{
    std::vector<sptrUnitCell>::iterator it;
    for (it = _unit_cells.begin(); it != _unit_cells.end(); ++it) {
        if ((*it)->name() == name) {
            _unit_cells.erase(it);
            --_last_index;
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
    std::string name = ohkl::kw_referenceUnitcell;
    UnitCell reference_cell{a, b, c, alpha * deg, beta * deg, gamma * deg};
    addUnitCell(name, reference_cell);
}

bool UnitCellHandler::checkAndAssignUnitCell(
    PeakCollection* peaks, AutoIndexer* auto_indexer, double length_tol, double angle_tol,
    std::string name)
{
    std::string ref_name = ohkl::kw_referenceUnitcell;
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
    peaks->setUnitCell(getSptrUnitCell(cellName));
}

std::vector<std::string> UnitCellHandler::getCompatibleSpaceGroups() const
{
    return getSptrUnitCell(ohkl::kw_acceptedUnitcell)->compatibleSpaceGroups();
}

std::string UnitCellHandler::generateUnitCellName()
{
    int n = 4; // number of digits
    std::string str = std::to_string(_last_index);
    if (str.size() > n) { //
        return "Please enter name for this unit cell";
    }
    return std::string("UnitCell") + std::string(n - str.size(), '0').append(str);
}

void UnitCellHandler::setLastIndex(unsigned int index)
{
    _last_index = index;
}

std::vector<UnitCell*> UnitCellHandler::getUnitCells() const
{
    std::vector<UnitCell*> cells;
    for (auto cell : _unit_cells)
        cells.push_back(cell.get());
    return cells;
}

std::vector<sptrUnitCell> UnitCellHandler::getSptrUnitCells() const
{
    std::vector<sptrUnitCell> cells;
    for (auto cell : _unit_cells)
        cells.push_back(cell);
    return cells;
}

} // namespace ohkl
