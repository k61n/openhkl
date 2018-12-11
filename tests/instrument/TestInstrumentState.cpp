#include <core/DataSet.h>
#include <core/Experiment.h>
#include <core/IDataReader.h>
#include <core/InstrumentState.h>
#include <core/NSXTest.h>

NSX_INIT_TEST

namespace nsx {
class UnitTest_DataSet {
public:
    static int run();
};
}

int nsx::UnitTest_DataSet::run()
{
    nsx::Experiment experiment("test", "BioDiff2500");

    nsx::sptrDataSet dataset(new nsx::DataSet("hdf", "gal3.hdf", experiment.diffractometer()));

    experiment.addData(dataset);

    auto detectorStates = dataset->_reader->detectorStates();
    auto sampleStates = dataset->_reader->sampleStates();

    for (size_t i = 0; i < 100 * (dataset->nFrames() - 1); ++i) {
        double frame = double(i) / 100.0;
        auto state = dataset->interpolatedState(frame);

        auto lframe = std::lround(std::floor(frame));
    }

    dataset->close();

    return 0;
}

int main()
{
    return nsx::UnitTest_DataSet::run();
}
