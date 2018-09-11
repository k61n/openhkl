#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/NSXTest.h>

NSX_INIT_TEST

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
    auto diff = expt->diffractometer();
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", diff));

    expt->addData(dataf);

    auto detectorStates = dataf->_reader->detectorStates();
    auto sampleStates = dataf->_reader->sampleStates();

    for (size_t i = 0; i < 100*(dataf->nFrames()-1); ++i) {
        double frame = double(i) / 100.0;
        auto state = dataf->interpolatedState(frame);

        auto lframe = std::lround(std::floor(frame));
    }
    return 0;
}

int main() 
{
    return nsx::UnitTest_DataSet::run();
}
