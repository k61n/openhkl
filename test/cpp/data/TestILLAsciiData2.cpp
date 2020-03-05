#include <iostream>

#include "base/utils/Units.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/ILLDataReader.h"
#include "test/cpp/catch.hpp"


TEST_CASE(__FILE__, "")
{
    try {
        nsx::ILLDataReader ill("D10_ascii_example", nsx::Diffractometer::create("D10"));
        std::size_t nFrames = ill.nFrames();
        CHECK(nFrames == 41);
        CHECK(ill.nRows() == 32);
        CHECK(ill.nCols() == 32);

        auto dat = ill.data(20);
        std::cout << dat << std::endl;

        CHECK(dat.sum() == 5646);
    } catch (std::exception& e) {
        FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        FAIL("unknown exception while loading data");
    }
}
