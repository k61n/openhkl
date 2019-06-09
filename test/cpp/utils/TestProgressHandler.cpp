#include "test/cpp/catch.hpp"
#include <string>
#include <vector>

#include "core/utils/ProgressHandler.h"
using namespace nsx;

TEST_CASE("test/utils/TestProgressHandler.cpp", "")
{

    ProgressHandler handler;

    // check that it handles nullptr without exception
    handler.setCallback(nullptr);

    // send it a dummy callback
    int count = 0;
    handler.setCallback([&count]() { ++count; });

    handler.setProgress(50);
    CHECK(handler.getProgress() == 50);
    CHECK(count == 1); // modified by callback

    handler.setStatus("progress handler test");
    CHECK(handler.getStatus() == "progress handler test");

    handler.log("progress handler log 1");
    handler.log("progress handler log 2");
    std::vector<std::string> log = handler.getLog();
    CHECK(log.size() == 2);
    CHECK(log[0] == "progress handler log 1");
    CHECK(log[1] == "progress handler log 2");

    CHECK(handler.getLog().size() == 0);

    handler.log(std::string("string log"));
    CHECK(handler.getLog()[0] == "string log");

    handler.abort();
    CHECK(handler.aborted() == true);

    // check correct behaviour under abort
    bool aborted = false;

    try {
        handler.setProgress(20);
    } catch (std::exception& e) {
        aborted = true;
    }
    CHECK(aborted == true);
}
