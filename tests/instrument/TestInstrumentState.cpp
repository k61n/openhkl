#include "core/data/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Experiment.h"
#include "core/data/IDataReader.h"
#include "core/instrument/InstrumentState.h"


namespace nsx {
class UnitTest_DataSet {
public:
    static int run();
};
} // namespace nsx

int nsx::UnitTest_DataSet::run()
{
    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");

    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.diffractometer()));

    experiment.addData(dataf);

    auto detectorStates = dataf->_reader->detectorStates();
    auto sampleStates = dataf->_reader->sampleStates();

    for (size_t i = 0; i < 100 * (dataf->nFrames() - 1); ++i) {
        double frame = double(i) / 100.0;
        auto state = dataf->interpolatedState(frame);

        auto lframe = std::lround(std::floor(frame));
    }
    return 0;
}
TEST_CASE("test/instrument/TestInstrumentState.cpp", "") {

    return nsx::UnitTest_DataSet::run();
}
