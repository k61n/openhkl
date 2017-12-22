#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/AutoIndexer.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Sample.h>

#include <nsxlib/ConvolutionKernel.h>
#include <nsxlib/KernelFactory.h>

#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/ErfInv.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>

namespace nsx {
class UnitTest_DataSet {
public:
    static int run();
};
}

int nsx::UnitTest_DataSet::run()
{
    nsx::DataReaderFactory factory;

    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", diff));

    expt->addData(dataf);

    auto detector_gonio = dataf->getDiffractometer()->getDetector()->getGonio();
    auto sample_gonio = dataf->getDiffractometer()->getSample()->getGonio();

    auto detectorStates = dataf->_reader->detectorStates();
    auto sampleStates = dataf->_reader->sampleStates();

    for (int i = 0; i < 100*(dataf->getNFrames()-1); ++i) {
        double frame = double(i) / 100.0;
        auto state = dataf->getInterpolatedState(frame);

        auto lframe = std::lround(std::floor(frame));

        auto detectorState = detectorStates[lframe].interpolate(detectorStates[lframe+1], frame-lframe);
        auto sampleState = sampleStates[lframe].interpolate(sampleStates[lframe+1], frame-lframe);
       
        
        auto detector_trans = detector_gonio->getHomMatrix(detectorState);
        auto sample_trans = sample_gonio->getHomMatrix(sampleState);

        auto sample_U = sample_trans.rotation();
        auto detector_U = detector_trans.rotation();

        NSX_CHECK_ASSERT( (sample_U-state.sampleOrientation).norm() < 2e-5);
        NSX_CHECK_ASSERT( (detector_U-state.detectorOrientation).norm() < 2e-5);

        auto detector_offset = detector_trans.translation();
        auto sample_position = sample_trans.translation();

        NSX_CHECK_ASSERT( (detector_offset-state.detectorOffset).norm() < 1e-12);
        NSX_CHECK_ASSERT( (sample_position-state.samplePosition).norm() < 1e-12);

        auto ki = state.ki().rowVector();
        auto ki0 = diff->getSource()->getSelectedMonochromator().getKi().rowVector();

        NSX_CHECK_ASSERT( (ki-ki0).norm() < 1e-10);
    }
 

    return 0;
}

int main() 
{
    return nsx::UnitTest_DataSet::run();
}
