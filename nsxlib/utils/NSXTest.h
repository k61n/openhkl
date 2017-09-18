#pragma once

#include <cmath>
#include <iostream>
#include <string>

namespace nsx {

class NSXTest {

public:

    //! Constructor
    NSXTest();

    //! Destructor
    ~NSXTest();

    //! Check that \p condition is true
    void testCheckAssert(bool condition, const std::string& pred, const std::string& filename, int lineno);

    //! Check that \p expected is equal to \p observed
    template <typename T>
    void testCheckEqual(T expected, T observed, const std::string& pred, const std::string& filename, int lineno);

    //! Check that \p expected is different from \p observed
    template <typename T>
    void testCheckNotEqual(T expected, T observed, const std::string& pred, const std::string& filename, int lineno);

    //! Check that \p expected is equal to \p observed with \p epsilon tolerance
    template <typename T>
    void testCheckClose(T expected, T observed, T epsilon, const std::string& pred, const std::string& filename, int lineno);

    //! Check that \p expected is different from \p observed with \p epsilon tolerance
    template <typename T>
    void testCheckNotClose(T expected, T observed, T epsilon, const std::string& pred, const std::string& filename, int lineno);

    //! Trigger a failure by increasing the number of registered failures
    void triggerFailure(const std::string& description, const std::string& filename, int lineno);

    //! Trigger a success by increasing the number of registered successes
    void triggerSuccess();

    //! Return the number of successes of the unit test
    int nSuccesses() const;

    //! Return the number of failures of the unit test
    int nFailures() const;

    //! Return the number of tests of the unit test
    int nTests() const;

    //! \brief Reset the number of failures and successes to 0
    //! This method is derived for testing the nsx unit test framework by itself. As such, it must not be used in any other nsxlib unit tests.
    void reset();

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
void NSXTest::testCheckNotEqual(T expected, T observed, const std::string& pred, const std::string& filename, int lineno) {

    if (expected != observed) {
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

template <typename T>
void NSXTest::testCheckNotClose(T expected, T observed, T epsilon, const std::string& pred, const std::string& filename, int lineno) {

    if (std::fabs(expected - observed) > epsilon) {
        ++_n_successes;
        return;
    }
    ++_n_failures;
    std::cout << filename << "(" << lineno << "): TEST `" << pred << "' FAILED" << std::endl;

}

static NSXTest all_tests;

} // end namespace nsx

#define NSX_TEST_CHECK_EXCEPTION(expression,error)                                                \
    try {                                                                                         \
        expression;                                                                               \
        nsx::all_tests.triggerFailure(#expression " is not throwing " #error, __FILE__,__LINE__); \
    } catch (const error& exception) {                                                            \
        nsx::all_tests.triggerSuccess();                                                          \
    }

#define NSX_TEST_CHECK_ANY_EXCEPTION(expression)                                          \
    try {                                                                                 \
        expression;                                                                       \
        nsx::all_tests.triggerFailure(#expression " is not throwing", __FILE__,__LINE__); \
    } catch (...) {                                                                       \
        nsx::all_tests.triggerSuccess();                                                  \
    }

#define NSX_TEST_CHECK_NO_EXCEPTION(expression)                                       \
    try {                                                                             \
        expression;                                                                   \
        nsx::all_tests.triggerSuccess();                                              \
    } catch (...) {                                                                   \
        nsx::all_tests.triggerFailure(#expression " is throwing", __FILE__,__LINE__); \
    }

#define NSX_TEST_CHECK_ASSERT(condition) nsx::all_tests.testCheckAssert(condition, #condition, __FILE__, __LINE__)
#define NSX_TEST_CHECK_EQUAL(expected,observed) nsx::all_tests.testCheckEqual(expected, observed, #expected" == " #observed, __FILE__, __LINE__)
#define NSX_TEST_CHECK_NOT_EQUAL(expected,observed) nsx::all_tests.testCheckNotEqual(expected, observed, #expected" != " #observed, __FILE__, __LINE__)
#define NSX_TEST_CHECK_CLOSE(expected,observed,epsilon) nsx::all_tests.testCheckClose(expected, observed, epsilon, "|"#expected" - " #observed"| < "#epsilon, __FILE__, __LINE__)
#define NSX_TEST_CHECK_NOT_CLOSE(expected,observed,epsilon) nsx::all_tests.testCheckNotClose(expected, observed, epsilon, "|"#expected" - " #observed"| > "#epsilon, __FILE__, __LINE__)
