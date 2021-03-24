//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/UnitCellHandler.h
//! @brief     Handles unit cell manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_UNITCELLHANDLER_H
#define NSX_CORE_EXPERIMENT_UNITCELLHANDLER_H

#include "base/utils/Units.h"
#include "core/data/DataTypes.h"
#include <map>
#include <stdexcept>
#include <string>

namespace nsx {

class AutoIndexer;
class PeakHandler;
class PeakCollection;
class UnitCell;

using CellMap = std::map<std::string, std::unique_ptr<UnitCell>>;

class UnitCellHandler {

 public:
    UnitCellHandler() = default;
    ~UnitCellHandler();
    UnitCellHandler(const UnitCellHandler& other);
    UnitCellHandler& operator=(const UnitCellHandler& other) = default;

 public: // Handling unit cells
    //! Get a pointer to the map of unit cells
    const CellMap* getCellMap() const;
    //! Add some data to the experiment
    void addUnitCell(const std::string& name, const UnitCell& unit_cell);
    //! Add a unit cell to the experiment via cell parameters (skip autoindexing step)
    void addUnitCell(
        const std::string& name, double a, double b, double c, double alpha, double beta,
        double gamma);
    //! Returns true if the experiment has a data
    bool hasUnitCell(const std::string& name) const;
    //! Get a list of loaded list names
    std::vector<std::string> getUnitCellNames() const;
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
        PeakCollection* peaks, AutoIndexer* auto_indexer, double length_tol, double angle_tol);
    //! Assign unit cell to a peak collection, compute Miller indices from q and cell
    void assignUnitCell(PeakCollection* peaks, std::string cellName = "accepted") const;
    //! Get space groups compatible with unit cell
    std::vector<std::string> getCompatibleSpaceGroups() const;

 private:
    CellMap _unit_cells;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_UNITCELLHANDLER_H
