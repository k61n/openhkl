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
    void testCheckAssert(bool condition, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that \p observed is equal to \p predicted
    template <typename T>
    void testCheckEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that \p observed is different from \p predicted
    template <typename T>
    void testCheckNotEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that \p observed is equal to \p predicted with \p epsilon tolerance
    template <typename T>
    void testCheckClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that \p observed is different from \p predicted with \p epsilon tolerance
    template <typename T>
    void testCheckNotClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Trigger a failure by increasing the number of registered failures
    void triggerFailure(bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Trigger a success by increasing the number of registered successes
    void triggerSuccess();

    //! Return the number of successes of the unit test
    int nSuccesses() const;

    //! Return the number of failures of the unit test
    int nFailures() const;

    //! Return the number of unit tests that haave been skipped
    int nSkipped() const;

    //! Return the number of tests of the unit test
    int nTests() const;

private:

    int _n_failures;

    int _n_successes;

    int _n_skipped;
};

template <typename T>
void NSXTest::testCheckEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno) {
    if (observed == predicted) {
        ++_n_successes;
    } else {
        if (expectedFailure) {
            ++_n_skipped;
        } else {
            ++_n_failures;
            std::cout << filename << "(" << lineno << "): TEST `" << description << "' FAILED" << std::endl;
        }
    }
}

template <typename T>
void NSXTest::testCheckNotEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno) {
    if (observed != predicted) {
        ++_n_successes;
    } else {
        if (expectedFailure) {
            ++_n_skipped;
        } else {
            ++_n_failures;
            std::cout << filename << "(" << lineno << "): TEST `" << description << "' FAILED" << std::endl;
        }
    }
}

template <typename T>
void NSXTest::testCheckClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno) {

    if (std::fabs(observed - predicted) < epsilon) {
        ++_n_successes;
    } else {
        if (expectedFailure) {
            ++_n_skipped;
        } else {
            ++_n_failures;
            std::cout << filename << "(" << lineno << "): TEST `" << description << "' FAILED" << std::endl;
        }
    }
}

template <typename T>
void NSXTest::testCheckNotClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno) {
    if (std::fabs(observed - predicted) > epsilon) {
        ++_n_successes;
    } else {
        if (expectedFailure) {
            ++_n_skipped;
        } else {
            ++_n_failures;
            std::cout << filename << "(" << lineno << "): TEST `" << description << "' FAILED" << std::endl;
        }
    }
}

static NSXTest all_tests;

} // end namespace nsx

#define NSX_CHECK_EXCEPTION(expression,error)                                                \
    try {                                                                                         \
        expression;                                                                               \
        nsx::all_tests.triggerFailure(false, #expression " is not throwing " #error, __FILE__,__LINE__); \
    } catch (const error& exception) {                                                            \
        nsx::all_tests.triggerSuccess();                                                          \
    }

#define NSX_CHECK_ANY_EXCEPTION(expression)                                          \
    try {                                                                                 \
        expression;                                                                       \
        nsx::all_tests.triggerFailure(false, #expression " is not throwing", __FILE__,__LINE__); \
    } catch (...) {                                                                       \
        nsx::all_tests.triggerSuccess();                                                  \
    }

#define NSX_CHECK_NO_EXCEPTION(expression)                                       \
    try {                                                                             \
        expression;                                                                   \
        nsx::all_tests.triggerSuccess();                                              \
    } catch (...) {                                                                   \
        nsx::all_tests.triggerFailure(false, #expression " is throwing", __FILE__,__LINE__); \
    }

// nsx unit test check for assertion
#define NSX_CHECK_ASSERT_1(condition) nsx::all_tests.testCheckAssert(condition, false, #condition, __FILE__, __LINE__)
#define NSX_CHECK_ASSERT_2(condition,expectedFailure) nsx::all_tests.testCheckAssert(condition, expectedFailure, #condition, __FILE__, __LINE__)
#define NSX_CHECK_ASSERT_X(x,A,B,FUNC, ...) FUNC
#define NSX_CHECK_ASSERT(...) NSX_CHECK_ASSERT_X(,##__VA_ARGS__,NSX_CHECK_ASSERT_2(__VA_ARGS__),NSX_CHECK_ASSERT_1(__VA_ARGS__))

// nsx unit test check for integral type equality
#define NSX_CHECK_EQUAL_1(observed,predicted) nsx::all_tests.testCheckEqual(observed, predicted, false, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_EQUAL_2(observed,predicted,expectedFailure) nsx::all_tests.testCheckEqual(observed, predicted, expectedFailure, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_EQUAL_X(x,A,B,C,FUNC, ...) FUNC
#define NSX_CHECK_EQUAL(...) NSX_CHECK_EQUAL_X(,##__VA_ARGS__,NSX_CHECK_EQUAL_2(__VA_ARGS__),NSX_CHECK_EQUAL_1(__VA_ARGS__))

// nsx unit test check for integral type inequality
#define NSX_CHECK_NOT_EQUAL_1(observed,predicted) nsx::all_tests.testCheckNotEqual(observed, predicted, false, #observed" != " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_NOT_EQUAL_2(observed,predicted,expectedFailure) nsx::all_tests.testCheckNotEqual(observed, predicted, expectedFailure, #observed" != " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_NOT_EQUAL_X(x,A,B,C,FUNC, ...) FUNC
#define NSX_CHECK_NOT_EQUAL(...) NSX_CHECK_NOT_EQUAL_X(,##__VA_ARGS__,NSX_CHECK_NOT_EQUAL_2(__VA_ARGS__),NSX_CHECK_NOT_EQUAL_1(__VA_ARGS__))

// nsx unit test check for floating type equality
#define NSX_CHECK_CLOSE_1(observed,predicted,epsilon) nsx::all_tests.testCheckClose(observed, predicted, epsilon, false, "|"#observed" - " #predicted"| < "#epsilon, __FILE__, __LINE__)
#define NSX_CHECK_CLOSE_2(observed,predicted,epsilon,expectedFailure) nsx::all_tests.testCheckClose(observed, predicted, epsilon, expectedFailure, "|"#observed" - " #predicted"| < "#epsilon, __FILE__, __LINE__)
#define NSX_CHECK_CLOSE_X(x,A,B,C,D,FUNC, ...) FUNC
#define NSX_CHECK_CLOSE(...) NSX_CHECK_CLOSE_X(,##__VA_ARGS__,NSX_CHECK_CLOSE_2(__VA_ARGS__),NSX_CHECK_CLOSE_1(__VA_ARGS__))

// nsx unit test check for floating type inequality
#define NSX_CHECK_NOT_CLOSE_1(observed,predicted,epsilon) nsx::all_tests.testCheckNotClose(observed, predicted, epsilon, false, "|"#observed" - " #predicted"| > "#epsilon, __FILE__, __LINE__)
#define NSX_CHECK_NOT_CLOSE_2(observed,predicted,epsilon,expectedFailure) nsx::all_tests.testCheckNotClose(observed, predicted, epsilon, expectedFailure, "|"#observed" - " #predicted"| > "#epsilon, __FILE__, __LINE__)
#define NSX_CHECK_NOT_CLOSE_X(x,A,B,C,D,FUNC, ...) FUNC
#define NSX_CHECK_NOT_CLOSE(...) NSX_CHECK_NOT_CLOSE_X(,##__VA_ARGS__,NSX_CHECK_NOT_CLOSE_2(__VA_ARGS__),NSX_CHECK_NOT_CLOSE_1(__VA_ARGS__))

// nsx failure trigger
#define NSX_FAIL_1(description) nsx::all_tests.triggerFailure(false, description,__FILE__,__LINE__)
#define NSX_FAIL_2(expectedFailure, description) nsx::all_tests.triggerFailure(expectedFailure, description,__FILE__,__LINE__)
#define NSX_FAIL_X(x,A,B,FUNC, ...) FUNC
#define NSX_FAIL(...) NSX_FAIL_X(,##__VA_ARGS__,NSX_FAIL_2(__VA_ARGS__),NSX_FAIL_1(__VA_ARGS__))
