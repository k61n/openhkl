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
#include "core/shape/PeakCollection.h"
#include "tables/crystal/UnitCell.h"
#include <QDebug>

namespace nsx {

UnitCellHandler::~UnitCellHandler() = default;

UnitCellHandler::UnitCellHandler(const UnitCellHandler& other)
{
    for (auto const& [name, cell] : other._unit_cells) {
        addUnitCell(name, *cell.get());
    }
}

const CellMap* UnitCellHandler::getCellMap() const
{
    return &_unit_cells;
}

void UnitCellHandler::addUnitCell(const std::string& name, const UnitCell& unit_cell)
{
    nsxlog(Level::Info, "UnitCellHandler::addUnitCell:", name, ":", unit_cell.toString());
    std::unique_ptr<UnitCell> ptr(new UnitCell(unit_cell));
    ptr->setName(name);
    _unit_cells.insert_or_assign(name, std::move(ptr));
}

void UnitCellHandler::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma)
{
    UnitCell cell{a, b, c, alpha * deg, beta * deg, gamma * deg};
    addUnitCell(name, cell);
}

void UnitCellHandler::addUnitCell(
    const std::string& name, double a, double b, double c, double alpha, double beta, double gamma,
    const std::string& space_group)
{
    UnitCell cell{a, b, c, alpha * deg, beta * deg, gamma * deg};
    cell.setSpaceGroup(space_group);
    addUnitCell(name, cell);
}

bool UnitCellHandler::hasUnitCell(const std::string& name) const
{
    return _unit_cells.find(name) != _unit_cells.end();
}

std::vector<std::string> UnitCellHandler::getUnitCellNames() const
{
    std::vector<std::string> names;
    for (CellMap::const_iterator it = _unit_cells.begin(); it != _unit_cells.end(); ++it) {
        names.push_back(it->second->name());
    }
    return names;
}

UnitCell* UnitCellHandler::getUnitCell(const std::string& name) const
{
    if (hasUnitCell(name)) {
        return _unit_cells.find(name)->second.get();
    }
    return nullptr;
}

void UnitCellHandler::removeUnitCell(const std::string& name)
{
    if (hasUnitCell(name)) {
        auto unit_cell = _unit_cells.find(name);
        unit_cell->second.reset();
        _unit_cells.erase(unit_cell);
    }
}

void UnitCellHandler::swapUnitCells(
    const std::string& old_cell_name, const std::string& new_cell_name,
    PeakHandler* peak_handler) const
{
    UnitCell* old_cell = getUnitCell(old_cell_name);
    UnitCell* new_cell = getUnitCell(new_cell_name);
    const auto* peak_collections = peak_handler->getPeakCollectionMap();

    std::map<std::string, std::unique_ptr<PeakCollection>>::const_iterator it;
    for (it = peak_collections->begin(); it != peak_collections->end(); ++it) {
        std::vector<Peak3D*> peaks = it->second->getPeakList();
        for (Peak3D* peak : peaks) {
            if (peak->unitCell() == old_cell)
                peak->setUnitCell(new_cell);
        }
    }
}

void UnitCellHandler::setReferenceCell(
    double a, double b, double c, double alpha, double beta, double gamma)
{
    std::string name = "reference";
    UnitCell reference_cell{a, b, c, alpha * deg, beta * deg, gamma * deg};
    addUnitCell(name, reference_cell);
}

bool UnitCellHandler::checkAndAssignUnitCell(
    PeakCollection* peaks, AutoIndexer* auto_indexer, double length_tol, double angle_tol)
{
    std::string name = "accepted";
    std::string ref_name = "reference";
    UnitCell* ref_cell = getUnitCell(ref_name);
    bool accepted = false;
    UnitCell* good_cell = auto_indexer->goodSolution(ref_cell, length_tol, angle_tol);
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
        peak->setUnitCell(getUnitCell(cellName));
        peak->setMillerIndices();
    }
}

std::vector<std::string> UnitCellHandler::getCompatibleSpaceGroups() const
{
    return getUnitCell("accepted")->compatibleSpaceGroups();
}

} // namespace nsx
