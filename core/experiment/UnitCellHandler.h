//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/UnitCellHandler.h
//! @brief     Handles unit cell manipulations for Experiment object
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_UNITCELLHANDLER_H
#define OHKL_CORE_EXPERIMENT_UNITCELLHANDLER_H

#include "base/utils/Units.h"
#include "core/data/DataTypes.h"
#include "core/raw/DataKeys.h"
#include "tables/crystal/UnitCell.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace ohkl {

class AutoIndexer;
class PeakHandler;
class PeakCollection;

using UnitCellList = std::vector<sptrUnitCell>;

class UnitCellHandler {

 public:
    UnitCellHandler();

 public: // Handling unit cells
    //! Get a pointer to the map of unit cells
    const UnitCellList* getCellList() const;
    //! Add a unit cell to the experiment
    void addUnitCell(const std::string& name, const UnitCell& unit_cell);
    //! Move a unique pointer to a unit cell to the experiment
    void addUnitCell(const std::string& name, sptrUnitCell unit_cell);
    //! Add a unit cell to the experiment via cell parameters (skip autoindexing step)
    void addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma);
    //! Add a user-defined unit cell to the experiment including space group
    void addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma, const std::string& space_group);
    //! Returns true if the experiment has a data
    bool hasUnitCell(const std::string& name) const;
    //! Get a list of loaded list names
    std::vector<std::string> getUnitCellNames() const;
    //! Returns the unit cell denoted by the name
    sptrUnitCell getSptrUnitCell(const std::string& name) const;
    //! Returns the unit cell denoted by the integer id
    sptrUnitCell getSptrUnitCell(const unsigned int id) const;
    //! Returns the unit cell denoted by the name
    UnitCell* getUnitCell(const std::string& name) const;
    //! Remove a data from the experiment
    void removeUnitCell(const std::string& name);
    //! Remove a data from the experiment
    void swapUnitCells(
        const std::string& old_cell, const std::string& new_cell, PeakHandler* peak_handler) const;
    //! Set the reference cell
    void setReferenceCell(double a, double b, double c, double alpha, double beta, double gamma);
    //! Get the number of peak lists
    int numUnitCells() const { return _unit_cells.size(); };
    //! Accept an autoindexer solution as the unit cell
    bool checkAndAssignUnitCell(
        PeakCollection* peaks, AutoIndexer* auto_indexer, double length_tol, double angle_tol,
        std::string name);
    //! Assign unit cell to a peak collection, compute Miller indices from q and cell
    void assignUnitCell(
        PeakCollection* peaks, std::string cellName = ohkl::kw_acceptedUnitcell) const;
    //! Get space groups compatible with unit cell
    std::vector<std::string> getCompatibleSpaceGroups() const;

    std::string generateUnitCellName();

    void setLastIndex(unsigned int index);
    std::vector<UnitCell*> getUnitCells() const;
    std::vector<sptrUnitCell> getSptrUnitCells() const;


 private:
    UnitCellList _unit_cells;
    const std::size_t _max_unit_cells = 1000;
    unsigned int _last_index = 0;
};

} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_UNITCELLHANDLER_H
