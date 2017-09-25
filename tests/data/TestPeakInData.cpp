#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/utils/NSXTest.h>

int main()
{
    nsx::DataReaderFactory factory;
    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;
    nsx::MetaData* metadata;

    try {
        diff = nsx::Diffractometer::build("D9");
        dataf = factory.create("", "D9_ascii_example", diff);

        dataf->open();

        metadata = dataf->getMetadata();
        NSX_CHECK_ASSERT(metadata != nullptr);
    }
    catch(std::exception& e) {
        NSX_FAIL(std::string("caught exception: ") + e.what());
    }
    catch (...) {
        NSX_FAIL("unknown exception");
    }
}
