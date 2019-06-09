#include "test/cpp/catch.hpp"

#include <Eigen/Dense>

#include "core/auto_indexing/AutoIndexer.h"
#include "core/crystal/UnitCell.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/CrystalTypes.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/geometry/DirectVector.h"
#include "core/geometry/ReciprocalVector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/integration/ShapeLibrary.h"
#include "core/loader/DataReaderFactory.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakFilter.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/utils/ProgressHandler.h"

TEST_CASE("test/crystal/TestFFTIndexing.cpp", "")
{

    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet data(factory.create("hdf", "gal3.hdf", experiment.diffractometer()));
    experiment.addData(data);

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

    Eigen::Matrix3d A;
    A << 45.0, 1.0, -2.0, -1.5, 36.0, -2.2, 1.25, -3, 50.0;
    nsx::UnitCell uc(A);
    uc.reduce(params.niggliReduction, params.niggliTolerance, params.gruberTolerance);
    uc = uc.applyNiggliConstraints();

    A = uc.basis();
    Eigen::Matrix3d BU = uc.reciprocalBasis();

    auto reflections = uc.generateReflectionsInShell(0.5, 100, 2.67);

    std::vector<nsx::ReciprocalVector> qs;

    for (auto index : reflections)
        qs.emplace_back(index.rowVector().cast<double>() * BU);

    auto events = data->events(qs);

    std::vector<nsx::sptrPeak3D> peaks;

    for (auto event : events) {
        nsx::sptrPeak3D peak(new nsx::Peak3D(data));
        Eigen::Vector3d center = {event._px, event._py, event._frame};

        // dummy shape
        try {
            peak->setShape(nsx::Ellipsoid(center, 1.0));
            peak->setSelected(true);
            peaks.push_back(peak);

            nsx::MillerIndex hkl(peak->q(), uc);

            CHECK(hkl.error().norm() < 1e-10);

        } catch (...) {
            // invalid shape, nothing to do
        }
    }

    CHECK(peaks.size() >= 5900);

    nsx::AutoIndexer indexer;

    for (auto peak : peaks)
        indexer.addPeak(peak);

    CHECK_NOTHROW(indexer.autoIndex(params));

    auto solutions = indexer.solutions();

    CHECK(solutions.size() > 1);
    CHECK(solutions.front().second > 99.9);

    auto fit_uc = solutions.front().first;

    Eigen::Matrix3d fit_A = fit_uc->basis();
    Eigen::Matrix3d E = fit_A.inverse() * A;

    // square because of the orientation issue
    CHECK((E * E - Eigen::Matrix3d::Identity()).norm() < 1e-10);
}
