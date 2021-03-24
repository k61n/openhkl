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

#include "core/algo/DataReaderFactory.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Qs2Events.h"

TEST_CASE("test/crystal/TestFFTIndexing.cpp", "")
{
    auto logger = std::make_shared<nsx::ProgressHandler>();
    logger->setCallback([logger]() {
        for (const auto& log : logger->getLog())
            std::cout << log << std::endl;
    });

    nsx::IndexerParameters params;
    params.maxdim = 70.0;
    params.nSolutions = 10;
    params.nVertices = 10000;
    params.subdiv = 30;
    params.indexingTolerance = 0.2;
    params.niggliTolerance = 1e-3;
    params.gruberTolerance = 4e-2;
    params.niggliReduction = false;
    params.minUnitCellVolume = 20.0;
    params.unitCellEquivalenceTolerance = 0.05;
    params.solutionCutoff = 10.0;

    // real basis of unit cell
    Eigen::Matrix3d basis;
    basis << 45.0, 1.0, -2.0, -1.5, 36.0, -2.2, 1.25, -3, 50.0;
    nsx::UnitCell uc(basis);
    uc.reduce(params.niggliReduction, params.niggliTolerance, params.gruberTolerance);
    uc = uc.applyNiggliConstraints();
    std::cout << "Basis:\n" << uc.basis() << std::endl;

    const auto reflections = uc.generateReflectionsInShell(0.5, 100, 2.67);
    std::vector<nsx::ReciprocalVector> qs(reflections.size());
    for (const nsx::MillerIndex& index : reflections) {
        // std::cout << "reflection: " << index << std::endl;
        qs.emplace_back(index.rowVector().cast<double>() * uc.reciprocalBasis());
    }

    nsx::Experiment experiment("test", "BioDiff2500");
    const nsx::sptrDataSet data(
        nsx::DataReaderFactory().create("hdf", "gal3.hdf", experiment.getDiffractometer()));
    experiment.addData(data);

    nsx::PeakCollection peak_collection;
    const auto events = nsx::algo::qs2events(qs, data->instrumentStates(), data->detector());
    for (const nsx::DetectorEvent& event : events) {
        // std::cout << "event x=" << event._px << " y=" << event._py
        //    << " frame=" << event._frame << " tof=" << event._tof
        //    << std::endl;
        nsx::Peak3D peak(data);
        const Eigen::Vector3d center = {event._px, event._py, event._frame};

        // dummy shape
        try {
            peak.setShape(nsx::Ellipsoid(center, 1.0));
            peak.setSelected(true);
            CHECK(nsx::MillerIndex(peak.q(), uc).error().norm() < 1e-10);
            peak_collection.push_back(peak);
        } catch (...) {
            // invalid shape, nothing to do
        }
    }
    CHECK(peak_collection.numberOfPeaks() >= 5700);

    nsx::AutoIndexer* auto_indexer = experiment.autoIndexer();
    auto_indexer->setHandler(logger);
    auto_indexer->setParameters(params);
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
