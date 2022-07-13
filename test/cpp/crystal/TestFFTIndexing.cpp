//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/crystal/TestFFTIndexing.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <iostream>

#include "core/experiment/Experiment.h"
#include "core/raw/DataKeys.h"
#include "core/data/DataSet.h"
#include "core/peak/Qs2Events.h"

TEST_CASE("test/crystal/TestFFTIndexing.cpp", "")
{
    auto logger = std::make_shared<ohkl::ProgressHandler>();
    logger->setCallback([logger]() {
        for (const auto& log : logger->getLog())
            std::cout << log << std::endl;
    });

    ohkl::Experiment experiment("test", "BioDiff2500");
    const ohkl::sptrDataSet dataset_ptr { std::make_shared<ohkl::DataSet>
          (ohkl::kw_datasetDefaultName, experiment.getDiffractometer()) };

    dataset_ptr->addDataFile("gal3.hdf", "nsx");
    dataset_ptr->finishRead();

    experiment.addData(dataset_ptr);

    ohkl::AutoIndexer* auto_indexer = experiment.autoIndexer();

    auto* params = auto_indexer->parameters();
    params->maxdim = 70.0;
    params->nSolutions = 10;
    params->nVertices = 10000;
    params->subdiv = 30;
    params->indexingTolerance = 0.2;
    params->niggliTolerance = 1e-3;
    params->gruberTolerance = 4e-2;
    params->niggliReduction = false;
    params->minUnitCellVolume = 20.0;
    params->unitCellEquivalenceTolerance = 0.05;
    params->solutionCutoff = 10.0;

    // real basis of unit cell
    Eigen::Matrix3d basis;
    basis << 45.0, 1.0, -2.0, -1.5, 36.0, -2.2, 1.25, -3, 50.0;
    ohkl::UnitCell uc(basis);
    uc.reduce(params->niggliReduction, params->niggliTolerance, params->gruberTolerance);
    uc = uc.applyNiggliConstraints();
    std::cout << "Basis:\n" << uc.basis() << std::endl;

    const auto reflections = uc.generateReflectionsInShell(0.5, 100, 2.67);
    std::vector<ohkl::ReciprocalVector> qs(reflections.size());
    for (const ohkl::MillerIndex& index : reflections) {
        // std::cout << "reflection: " << index << std::endl;
        qs.emplace_back(index.rowVector().cast<double>() * uc.reciprocalBasis());
    }

    ohkl::PeakCollection peak_collection;
    const auto events =
        ohkl::algo::qVectorList2Events(qs, dataset_ptr->instrumentStates(), dataset_ptr->detector(), dataset_ptr->nFrames());
    for (const ohkl::DetectorEvent& event : events) {
        // std::cout << "event x=" << event.px << " y=" << event.py
        //    << " frame=" << event.frame << " tof=" << event.tof
        //    << std::endl;
        ohkl::Peak3D peak(dataset_ptr);
        const Eigen::Vector3d center = {event.px, event.py, event.frame};

        // dummy shape
        try {
            peak.setShape(ohkl::Ellipsoid(center, 1.0));
            peak.setSelected(true);
            CHECK(ohkl::MillerIndex(peak.q(), uc).error().norm() < 1e-10);
            peak_collection.push_back(peak);
        } catch (...) {
            // invalid shape, nothing to do
        }
    }
    CHECK(peak_collection.numberOfPeaks() >= 5700);

    auto_indexer->setHandler(logger);
    auto_indexer->autoIndex(peak_collection.getPeakList());

    const auto solutions = auto_indexer->solutions();
    CHECK(solutions.size() > 1);
    CHECK(solutions.front().second > 99.9);

    const Eigen::Matrix3d autoBasis = solutions.front().first->basis();
    std::cout << "Basis:\n" << autoBasis << std::endl;

    // check for identity
    const Eigen::Matrix3d E = autoBasis.inverse() * uc.basis();
    // square because of the orientation issue
    CHECK((E * E - Eigen::Matrix3d::Identity()).norm() < 1e-10);
}
