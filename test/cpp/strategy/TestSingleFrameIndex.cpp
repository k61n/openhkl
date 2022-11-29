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

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/utils/Units.h"
#include "core/convolve/Convolver.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"
#include "test/cpp/catch.hpp"

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
        "CrChiA_c01runab_28605.raw",
        "CrChiA_c01runab_28606.raw",
        "CrChiA_c01runab_28607.raw",
        "CrChiA_c01runab_28608.raw",
        "CrChiA_c01runab_28609.raw",
        "CrChiA_c01runab_28610.raw",
        "CrChiA_c01runab_28611.raw",
        "CrChiA_c01runab_28612.raw",
        "CrChiA_c01runab_28613.raw"};

        ohkl::Experiment experiment("test", "BioDiff2500");

    ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>(ohkl::kw_datasetDefaultName, experiment.getDiffractometer());

    ohkl::RawDataReaderParameters data_params;
    data_params.wavelength = 2.669;
    data_params.delta_omega = 0.3;
    data->setRawReaderParameters(data_params);
    for (const auto& filename : filenames)
        data->addRawFrame(filename);
    data->finishRead();
    experiment.addData(data);

    // mask edges and beam stop
    ohkl::AABB box1 = {{0, 0, -1}, {300, 900, 1}};
    ohkl::AABB box2 = {{2200, 0, -1}, {2500, 900, 1}};
    ohkl::AABB ellipse = {{1200, 400, -1}, {1300, 500, 1}};
    data->addMask(new ohkl::BoxMask(box1));
    data->addMask(new ohkl::BoxMask(box2));
    data->addMask(new ohkl::EllipseMask(ellipse));

    auto* finder = experiment.peakFinder2D();
    auto* finder_params = finder->parameters();
    finder_params->threshold = 80;
    finder->setData(data);
    finder->setConvolver(ohkl::ConvolutionKernelType::Annular);
    finder->find(0);

    std::vector<ohkl::Peak3D*> found_peaks = finder->getPeakList(0);

    data->adjustDirectBeam(-2.00, -2.10);
    // TODO: reimplement use of a single file and state
    // ohkl::InstrumentState* state = &data->instrumentStates().at(0);

    auto* indexer = experiment.autoIndexer();
    auto* indexer_params = indexer->parameters();
    indexer_params->d_min = 1.5;
    indexer_params->d_max = 50.0;
    indexer_params->nVertices = 10000;

    ohkl::UnitCell reference_cell;
    reference_cell.setParameters(
        57.5, 65.7, 85.4, 90.0 * ohkl::deg, 90.0 * ohkl::deg, 90.0 * ohkl::deg);
    reference_cell.setSpaceGroup(ohkl::SpaceGroup{"P 21 21 21"});

    indexer->autoIndex(found_peaks);
    ohkl::sptrUnitCell best_cell = indexer->solutions().at(0).first;
    std::cout << reference_cell.toString() << std::endl;
    std::cout << best_cell->toString() << std::endl;
    CHECK(best_cell->isSimilar(&reference_cell, 0.5, 0.1));
}
