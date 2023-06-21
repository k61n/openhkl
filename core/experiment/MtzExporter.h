//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/MtzExporter.h
//! @brief     Handles project export to mtz format
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef MTZ_EXPORTER_H
#define MTZ_EXPORTER_H

#include "base/utils/LogLevel.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "tables/crystal/UnitCell.h"

#include "3rdparty/ccp4/cmtzlib.h"
#include "3rdparty/ccp4/mtzdata.h"

#include <string.h>
#include <string>
#include <vector>

namespace ohkl {

class MergedPeakCollection;
class PeakCollection;
class PeakMerger;

class MtzExporter {
 public:
    MtzExporter(
        MergedPeakCollection* merged_data, sptrDataSet data, sptrUnitCell cell, bool merged,
        bool sum_intensities, std::string comment);
    ~MtzExporter();
    //! Builds whole MtzData structure from ohkl data
    void buildMtzData();
    //! Export build MtzData to file
    bool exportToFile(std::string filename);
    //! add History
    void addHistory(std::string line);

 private:
    //! Build top level Mtz structure
    void buildMtz();
    //! Build MtzSet data set structure(s)
    void buildMtzSet();
    //! Build columns for reflections
    void buildMtzCols();
    //! Build Mtz Xtal Structure (Crystall)
    void buildXTAL();
    //! Build history structure
    void buildHistory();
    //! Build Mtz SymmetryInfo structure
    void buildSyminfo();
    //! Build Mtz MNF structure (missing refelction)
    void buildMNF();
    //! Building Mtz batch structures (N.B. batch = image in this context)
    void buildBatches();
    //! Create a column for the reflection table
    CMtz::MTZCOL* CreateMtzCol(
        std::string name, std::string label, int grp, int set_id, int active, int src);

 private:
    // ohkl data structures
    MergedPeakCollection* _merged_data;
    sptrDataSet _ohkl_data;
    UnitCell* _ohkl_cell;
    //! Whether to export merged (true) or unmerged (false) peaks
    bool _merged;
    //! Whether to use pixel sum (true) or profile intensities
    bool _sum_intensities;

    std::vector<std::string> _history;
    std::string _comment;
    std::string _peakcollection_name;

    // The necessary Mtz data structures
    // MTZ -> XTAL** -> MTZSET** -> MTZCOL** (contains data)
    // MTZ -> BATCH*
    // MTZ -> SymInfo
    // declared in 3rdparty/ccp4/mtzdata.h"

    // main mtz data structure
    CMtz::MTZ* _mtz;

    CMtz::MTZXTAL* _mtz_xtal;
    std::vector<CMtz::MTZCOL*> _mtz_cols;
};
}
#endif // OHKL_MTZEXPORTER_H
