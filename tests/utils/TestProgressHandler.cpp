#define BOOST_TEST_MODULE "Test ProgressHandler class"
#define BOOST_TEST_DYN_LINK

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <nsxlib/utils/ProgressHandler.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_ProgressHandler)
{
    ProgressHandler handler;

    // check that it handles nullptr without exception
    handler.setCallback(nullptr);

    // send it a dummy callback
    int count = 0;
    handler.setCallback([&count] () {++count;});

    handler.setProgress(50);
    BOOST_CHECK(handler.getProgress() == 50);
    BOOST_CHECK(count == 1); // modified by callback

    handler.setStatus("progress handler test");
    BOOST_CHECK(handler.getStatus() == "progress handler test");

    handler.log("progress handler log 1");
    handler.log("progress handler log 2");
    std::vector<std::string> log = handler.getLog();
    BOOST_CHECK(log.size() == 2);
    BOOST_CHECK(log[0] == "progress handler log 1");
    BOOST_CHECK(log[1] == "progress handler log 2");

    BOOST_CHECK(handler.getLog().size() == 0);

    handler.log(std::string("string log"));
    BOOST_CHECK(handler.getLog()[0] == "string log");

    handler.abort();
    BOOST_CHECK(handler.aborted() == true);

    // check correct behaviour under abort
    bool aborted = false;

    try {
        handler.setProgress(20);
    }
    catch (std::exception& e) {
        aborted = true;
    }
    BOOST_CHECK(aborted == true);
}
