//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestSingleFrameIndex.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


TEST_CASE("test/data/TestSingleFrameIndex.cpp", "")
{
    // TODO: indexing only seems to work when multiple frames are added. Look into this!
    const std::vector<std::string> filenames = {
        "CrChiA_c01runab_28603.raw",
        "CrChiA_c01runab_28604.raw",
        "CrChiA_c01runab_28605.raw"};

    ohkl::Experiment experiment("CrChiA", "BioDiff");

    ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>("CrChiA", experiment.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::RAW;
    data_params.wavelength = 2.669;
    data_params.delta_omega = 0.3;
    data->setImageReaderParameters(data_params);
    for (const auto& filename : filenames)
        data->addFrame(filename, ohkl::DataFormat::RAW);
    data->finishRead();
    experiment.addData(data);

    auto* finder = experiment.peakFinder2D();
    auto* finder_params = finder->parameters();
    finder_params->threshold = 60;
    finder_params->kernel = ohkl::ImageFilterType::Annular;
    std::map<std::string, double> filter_params = {{"r1", 5}, {"r2", 10}, {"r3", 15}};
    finder->setFilterParameters(filter_params);
    finder->setData(data);
    finder->find(0);

    std::vector<ohkl::Peak3D*> found_peaks = finder->getPeakList(0);
    std::cout << found_peaks.size() << " peaks found" << std::endl;

    data->adjustDirectBeam(-1.00, -2.00);
    // TODO: reimplement use of a single file and state
    // ohkl::InstrumentState* state = &data->instrumentStates().at(0);

    auto* indexer = experiment.autoIndexer();
    auto* indexer_params = indexer->parameters();
    indexer_params->d_min = 1.5;
    indexer_params->d_max = 50.0;
    indexer_params->nVertices = 10000;
    indexer_params->peaks_integrated = false;

    ohkl::UnitCell reference_cell;
    reference_cell.setParameters(
        57.5, 65.7, 85.4, 90.0 * ohkl::deg, 90.0 * ohkl::deg, 90.0 * ohkl::deg);
    reference_cell.setSpaceGroup(ohkl::SpaceGroup{"P 21 21 21"});

    indexer->autoIndex(found_peaks, data);
    std::cout << indexer->solutionsToString() << std::endl;
    ohkl::sptrUnitCell best_cell = indexer->solutions().at(0).first;
    std::cout << reference_cell.toString() << std::endl;
    std::cout << best_cell->toString() << std::endl;
    CHECK(best_cell->isSimilar(&reference_cell, 0.5, 0.1));
}
