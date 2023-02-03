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

class MergedData;
class PeakCollection;
class PeakMerger;

class MtzExporter {
 public:
    //! Constructor
    MtzExporter(
        MergedData* merged_data, sptrDataSet data, sptrUnitCell cell, bool merged,
        bool sum_intensities, std::string comment);
    //! Destructor
    ~MtzExporter();
    //! Builds whole MtzData structure from ohkl data
    void buildMtzData();
    //! Export build MtzData to file
    bool exportToFile(std::string filename);
    //! add History
    void addHistory(std::string line);

 private:
    //! Build the MtzSet datastructure(s)
    void buildMtzSet();
    //! Build the MtzCol datastructure(s)
    void buildMtzCols();
    //! Build the Mtz main structure
    void buildMtz();
    //! Build Mtz Xtal Structure (Crystall)
    void buildXTAL();
    //! Build History structure
    void buildHistory();
    //! Build Mtz SymmetryInfo structure
    void buildSyminfo();
    //! Build Mtz MNF structure (missing refelction)
    void buildMNF();
    //! Building Mtz Batch strucutre
    void buildBatch();
    //! Handles the details of creating MTZCol
    CMtz::MTZCOL* CreateMtzCol(
        std::string name, std::string label, int grp, int set_id, int active, int src);

 private:
    //! Merger object to process the merge if necessary
    PeakMerger* _merger;

    // ohkl data structures
    MergedData* _merged_data;
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

    /* main mtz data structure */
    CMtz::MTZ* _mtz_data;

    // handles for sub structures
    CMtz::MTZXTAL* _mtz_xtal;
    std::vector<CMtz::MTZCOL*> _mtz_cols;
    std::vector<CMtz::MTZSET*> _mtz_sets;
};
}
#endif // OHKL_MTZEXPORTER_H
