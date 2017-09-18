#include "NSXTest.h"

#include <cstdlib>

namespace nsx {

NSXTest::NSXTest() : _n_failures(0), _n_successes(0)
{
}

NSXTest::~NSXTest()
{
    std::cout << "------------------------------------------------------\n";
    std::cout << "test completed: " << _n_successes << " successes; " << _n_failures << " failures.";
    std::cout << std::endl;

    if (_n_failures > 0) {
        std::exit(1);
    }
}

void NSXTest::testCheckAssert(bool condition, const std::string& pred, const std::string& filename, int lineno) {

    if (condition) {
        ++_n_successes;
        return;
    }
    ++_n_failures;
    std::cout << filename << "(" << lineno << "): TEST `" << pred << "' FAILED" << std::endl;

}

void NSXTest::triggerFailure(const std::string& description, const std::string& filename, int lineno) {

    std::cout << filename << "(" << lineno << "): TEST `" << description << "' FAILED" << std::endl;
    ++_n_failures;

}

void NSXTest::triggerSuccess() {

    ++_n_successes;

}

int NSXTest::nFailures() const
{
    return _n_failures;
}

int NSXTest::nSuccesses() const
{
    return _n_successes;
}

int NSXTest::nTests() const
{
    return _n_failures + _n_successes;
}

void NSXTest::reset()
{
    _n_failures = 0;
    _n_successes = 0;
}

} // end namespace nsx
