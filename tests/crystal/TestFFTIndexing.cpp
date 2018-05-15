#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/AutoIndexer.h>
#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/DirectVector.h>
#include <nsxlib/ErfInv.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Sample.h>
#include <nsxlib/ShapeLibrary.h>
#include <nsxlib/StrongPeakIntegrator.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

NSX_INIT_TEST

int main()
{
    nsx::DataReaderFactory factory;

    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
    nsx::sptrDataSet data(factory.create("hdf", "gal3.hdf", diff));
    expt->addData(data);

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

#if 0
struct IndexerParameters {
    double maxdim = 200.0;
    int nSolutions = 10;
    int nVertices = 10000;
    int subdiv = 30;
    double indexingTolerance = 0.2;
    double niggliTolerance = 1e-3;
    double gruberTolerance = 4e-2;
    bool niggliReduction = false;
    double minUnitCellVolume = 20.0;
    double unitCellEquivalenceTolerance = 0.05;
    double solutionCutoff = 10.0;
};
#endif


    Eigen::Matrix3d A;
    A << 45.0, 1.0, -2.0, -1.5, 36.0, -2.2, 1.25, -3, 50.0;
    nsx::UnitCell uc(A);
    uc.reduce(params.niggliReduction, params.niggliTolerance, params.gruberTolerance);
    uc = uc.applyNiggliConstraints();
    
    A = uc.basis();
    Eigen::Matrix3d BU = uc.reciprocalBasis();

    auto reflections = uc.generateReflectionsInShell(0.5, 100, 2.67);

    std::vector<nsx::ReciprocalVector> qs;

    for (auto index: reflections) {
        qs.emplace_back(index.rowVector().cast<double>()*BU);
    }

    auto events = data->getEvents(qs);

    std::vector<nsx::sptrPeak3D> peaks;

     for (auto event: events) {
        nsx::sptrPeak3D peak(new nsx::Peak3D(data));
        Eigen::Vector3d center = {event._px, event._py, event._frame};

        // dummy shape
        try {
            peak->setShape(nsx::Ellipsoid(center, 1.0));
            peak->setSelected(true);
            peaks.push_back(peak);

            nsx::MillerIndex hkl(peak->q(), uc);

            NSX_CHECK_ASSERT(hkl.error().norm() < 1e-10);


        } catch(...) {
            // invalid shape, nothing to do
        }
    }

    NSX_CHECK_ASSERT(peaks.size() >= 5900);

    nsx::AutoIndexer indexer;

    for (auto peak: peaks) {
        indexer.addPeak(peak);
    }

   
    NSX_CHECK_NO_THROW(indexer.autoIndex(params));

    auto solutions = indexer.getSolutions();

    NSX_CHECK_ASSERT(solutions.size() > 1);
    NSX_CHECK_ASSERT(solutions.front().second > 99.9);

    auto fit_uc = solutions.front().first;

    Eigen::Matrix3d fit_A = fit_uc->basis();

    // orientation issue
    if (fit_A.determinant()*A.determinant() < 0) {
        fit_A *= -1;
    }

    NSX_CHECK_ASSERT((A-fit_A).norm() < 1e-12);

    std::cout << A << "\n-----------------------------" << std::endl;
    std::cout << fit_A << std::endl;


    return 0;
}
