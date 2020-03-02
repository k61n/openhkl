#include "test/cpp/catch.hpp"

#include <Eigen/Dense>

#include "core/algo/DataReaderFactory.h"
#include "core/algo/Qs2Events.h"
#include "core/experiment/Experiment.h"

TEST_CASE("test/crystal/TestFFTIndexing.cpp", "")
{
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

    Eigen::Matrix3d M;
    M << 45.0, 1.0, -2.0, -1.5, 36.0, -2.2, 1.25, -3, 50.0;
    nsx::UnitCell C(M);
    C.reduce(params.niggliReduction, params.niggliTolerance, params.gruberTolerance);
    const nsx::UnitCell uc = C.applyNiggliConstraints();

    std::vector<nsx::ReciprocalVector> qs;
    const Eigen::Matrix3d BU = uc.reciprocalBasis();
    const auto reflections = uc.generateReflectionsInShell(0.5, 100, 2.67);
    for (const nsx::MillerIndex& index : reflections)
        qs.emplace_back(index.rowVector().cast<double>() * BU);

    nsx::Experiment experiment("test", "BioDiff2500");
    const nsx::sptrDataSet data(
        nsx::DataReaderFactory().create("hdf", "gal3.hdf", experiment.diffractometer()));
    experiment.addData(data);

    nsx::PeakCollection peak_collection;
    const auto events = nsx::algo::qs2events(qs, data->instrumentStates(), data->detector());
    for (const nsx::DetectorEvent& event : events) {
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
    CHECK(peak_collection.numberOfPeaks() >= 5900);

    nsx::AutoIndexer *const auto_indexer = experiment.autoIndexer();
    auto_indexer->setParameters(params);
    auto_indexer->autoIndex(peak_collection.getPeakList());

    const auto solutions = auto_indexer->solutions();
    CHECK(solutions.size() > 1);
    CHECK(solutions.front().second > 99.9);

    const Eigen::Matrix3d autoBasis = solutions.front().first->basis();
    const Eigen::Matrix3d E = autoBasis.inverse() * uc.basis();

    // square because of the orientation issue
    CHECK((E * E - Eigen::Matrix3d::Identity()).norm() < 1e-10);
}
