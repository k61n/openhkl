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

#include <string>
#include <string.h>
#include <vector>

#include "3rdparty/ccp4/cmtzlib.h"
#include "3rdparty/ccp4/mtzdata.h"

#include "core/experiment/Experiment.h"
 
#include "base/utils/Logger.h"
#include "base/utils/LogLevel.h"
 
#include "core/statistics/MergedPeak.h"
#include "core/statistics/MergedData.h"

namespace ohkl { 

class MtzExporter{
    public:
        //! Constructor
        MtzExporter(ohkl::Experiment* expt, std::string dataset_name, std::string peakcollection_name,
            bool use_merged_data, std::string comment, ohkl::MergedData* merged_data);
        //! Deconstructor
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
        CMtz::MTZCOL* CreateMtzCol(std::string name, std::string label, int grp, 
            int set_id, int active, int src);

        void process( );

    private: 
        bool _use_merged_data; // 0 = unmerged, 1 = merged
        //ohkl data structures
        ohkl::Experiment* _expt;
        ohkl::sptrDataSet _ohkl_data;
        ohkl::UnitCell*   _ohkl_uc;
        ohkl::MergedData* _ohkl_merged_data;

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