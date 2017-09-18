#pragma once

#include <cmath>
#include <iostream>
#include <string>

namespace nsx {

class NSXTest {

public:

    NSXTest();

    ~NSXTest();

    void testCheckAssert(bool condition, const std::string& pred, const std::string& filename, int lineno);

    template <typename T>
    void testCheckEqual(T expected, T observed, const std::string& pred, const std::string& filename, int lineno);

    template <typename T>
    void testCheckClose(T expected, T observed, T epsilon, const std::string& pred, const std::string& filename, int lineno);

    void triggerFailure(const std::string& description, const std::string& filename, int lineno);

    void triggerSuccess();

    int nSuccesses() const;

    int nFailures() const;

    int nTests() const;

private:

    int _n_failures;

    int _n_successes;
};

template <typename T>
void NSXTest::testCheckEqual(T expected, T observed, const std::string& pred, const std::string& filename, int lineno) {

    if (expected == observed) {
        ++_n_successes;
        return;
    }
    ++_n_failures;
    std::cout << filename << "(" << lineno << "): TEST `" << pred << "' FAILED" << std::endl;

}

template <typename T>
void NSXTest::testCheckClose(T expected, T observed, T epsilon, const std::string& pred, const std::string& filename, int lineno) {

    if (std::fabs(expected - observed) < epsilon) {
        ++_n_successes;
        return;
    }
    ++_n_failures;
    std::cout << filename << "(" << lineno << "): TEST `" << pred << "' FAILED" << std::endl;

}

static NSXTest all_tests;

} // end namespace nsx

#define NSX_TEST_CHECK_THROW(expression,error) \
    try {                                  \
        expression;                        \
        nsx::all_tests.triggerFailure(#expression " is not throwing " #error, __FILE__,__LINE__);    \
    } catch (const error& exception) {     \
        nsx::all_tests.triggerSuccess();        \
    }

#define NSX_TEST_CHECK_NOT_THROW(expression)   \
    try {                                  \
        expression;                        \
        nsx::all_tests.triggerSuccess();        \
    } catch (...) {                        \
        nsx::all_tests.triggerFailure(#expression " is throwing ", __FILE__,__LINE__);    \
    }

#define NSX_TEST_CHECK_ASSERT(condition) nsx::all_tests.testCheckAssert(condition, #condition, __FILE__, __LINE__)
#define NSX_TEST_CHECK_EQUAL(expected,observed) nsx::all_tests.testCheckEqual(expected, observed, #expected" == " #observed, __FILE__, __LINE__)
#define NSX_TEST_CHECK_CLOSE(expected,observed,epsilon) nsx::all_tests.testCheckClose(expected, observed, epsilon, "|"#expected" - " #observed"| <"#epsilon, __FILE__, __LINE__)
