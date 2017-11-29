#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

namespace nsx {

//! \class NSXTest
//! \brief This class implements an unit test framework.
//! It implements standards test cases such as:
//!
//! - Assertion (a.k.a boolean equality)
//! - Inequality/Equality
//! - Values close to each other
//! - Value close to zero
//! - Any/specific exception throw/not throw
//!
//! For each of this standard cases, a macros have been defined for convenience purpose.
class NSXTest {

public:

    //! Constructor.
    NSXTest();

    //! Copy constructor.
    NSXTest(const NSXTest& other)=default;

    //! Copy-assignment operator.
    NSXTest& operator=(const NSXTest& other)=default;

    //! Destructor.
    ~NSXTest();

    //! Check that an assert is true.
    //! \param [in] condition The assertion to be tested.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    void testCheckAssert(bool condition, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check for the equality between two integral-type values.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check for the inequality between two integral-type values.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckNotEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that a value is greater than another one.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckGreaterThan(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that a value is greater than or equal to another one.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckGreaterThanOrEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that a value is lower than another one.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckLowerThan(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that a value is greater than or equal to another one.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckLowerThanOrEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that two values are close to each other within a given tolerance.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] epsilon The tolerance value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that two values are not close to each other within a given tolerance.
    //! \param [in] observed The observed value.
    //! \param [in] predicted The predicted value.
    //! \param [in] epsilon The tolerance value. by increasing the number of registered failures
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckNotClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that a given value is close to zero within a given tolerance.
    //! \param [in] observed The observed value.
    //! \param [in] epsilon The tolerance value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckSmall(T observed, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Check that a given value is not close to zero within a given tolerance.
    //! \param [in] observed The observed value.
    //! \param [in] epsilon The tolerance value.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    template <typename T>
    void testCheckNotSmall(T observed, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Trigger a failure.
    //! \param [in] expectedFailure If \p true this test will not be counted as a failure.
    //! \param [in] description The message to be written in case of failure.
    //! \param [in] filename The file in which the unit test was run.
    //! \param [in] filename The line number at which the unit test was run.
    void triggerFailure(bool expectedFailure, const std::string& description, const std::string& filename, int lineno);

    //! Trigger a success.
    void triggerSuccess();

    //! Return the number of successes of the unit test
    //! \return the number of successful tests
    int nSuccesses() const;

    //! Return the number of failures of the unit test
    //! \return the number of failing tests
    int nFailures() const;

    //! Return the number of unit tests that have been skipped
    //! \return the number of skipped tests
    int nSkipped() const;

    //! Return the number of tests of the unit test
    //! \return the total number of tests
    int nTests() const;

private:

    //! Performs a floating division.
    //! Adapted from boost source code.
    template <typename T>
    T safeFloatingDivision(T f1, T f2);

    //! The number of failing tests.
    int _n_failures;

    //! The number of successful tests.
    int _n_successes;

    //! The number of skipped tests.
    int _n_skipped;
};

template <typename T>
void NSXTest::testCheckEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
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
void NSXTest::testCheckNotEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
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
void NSXTest::testCheckGreaterThan(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    if (observed > predicted) {
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
void NSXTest::testCheckGreaterThanOrEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    if (observed >= predicted) {
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
void NSXTest::testCheckLowerThan(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    if (observed < predicted) {
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
void NSXTest::testCheckLowerThanOrEqual(T observed, T predicted, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    if (observed <= predicted) {
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
void NSXTest::testCheckClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    const T diff = std::fabs(observed - predicted);

    const T d1 = safeFloatingDivision(diff, std::fabs(predicted));
    const T d2 = safeFloatingDivision(diff, std::fabs(observed));

    const T fraction = 0.01*epsilon;

    if (d1 < fraction && d2 < fraction) {
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
void NSXTest::testCheckNotClose(T observed, T predicted, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    const T diff = std::fabs(observed - predicted);

    const T d1 = safeFloatingDivision(diff, std::fabs(predicted));
    const T d2 = safeFloatingDivision(diff, std::fabs(observed));

    const T fraction = 0.01*epsilon;

    if (d1 > fraction && d2 > fraction) {
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
void NSXTest::testCheckSmall(T observed, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    if (std::fabs(observed) < epsilon) {
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
void NSXTest::testCheckNotSmall(T observed, T epsilon, bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{
    if (std::fabs(observed) > epsilon) {
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
T NSXTest::safeFloatingDivision(T f1, T f2 )
{
    // Avoid overflow.
    if( (f2 < static_cast<T>(1))  && (f1 > f2*std::numeric_limits<T>::max()) )
        return std::numeric_limits<T>::max();

    // Avoid underflow.
    if( (f1 == static_cast<T>(0)) ||
        ((f2 > static_cast<T>(1)) && (f1 < f2*std::numeric_limits<T>::min())) )
        return static_cast<T>(0);

    return f1/f2;
}


NSXTest&  allTests();

} // end namespace nsx

#define NSX_CHECK_THROW(expression,error)                                                \
    try {                                                                                         \
        expression;                                                                               \
        nsx::allTests().triggerFailure(false, #expression " is not throwing " #error, __FILE__,__LINE__); \
    } catch (const error& exception) {                                                            \
        nsx::allTests().triggerSuccess();                                                          \
    }

#define NSX_CHECK_THROW_ANY(expression)                                          \
    try {                                                                                 \
        expression;                                                                       \
        nsx::allTests().triggerFailure(false, #expression " is not throwing", __FILE__,__LINE__); \
    } catch (...) {                                                                       \
        nsx::allTests().triggerSuccess();                                                  \
    }

#define NSX_CHECK_NO_THROW(expression)                                       \
    try {                                                                             \
        expression;                                                                   \
        nsx::allTests().triggerSuccess();                                              \
    } catch (...) {                                                                   \
        nsx::allTests().triggerFailure(false, #expression " is throwing", __FILE__,__LINE__); \
    }

// Hack for solving a bug with the way Visual Studio expand the variadic macro
// See https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
#define EXPAND(x) x

// Hack to avoid the following warning due to the missing trailing comma for variadic argument macro
// warning: ISO C++11 requires at least one argument for the "..." in a variadic macro
// This warning occurred with e.g. CHECK_ASSERT(true) when the code is compiled with -Wpedantic
// Without that hack, to remove the warning, the macros should have been called using CHECK_ASSERT(true,)
// Based on https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros/3048361#3048361
#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define GET_5TH_ARG(arg1, arg2, arg3, arg4, arg5, ...) arg5

// nsx unit test check for assertion
#define NSX_CHECK_ASSERT_1(condition) nsx::allTests().testCheckAssert(condition, false, #condition, __FILE__, __LINE__)
#define NSX_CHECK_ASSERT_2(condition,expectedFailure) nsx::allTests().testCheckAssert(condition, expectedFailure, #condition, __FILE__, __LINE__)
#define NSX_CHECK_ASSERT_X(...) EXPAND(GET_3RD_ARG(__VA_ARGS__,NSX_CHECK_ASSERT_2, NSX_CHECK_ASSERT_1,))
#define NSX_CHECK_ASSERT(...) EXPAND(NSX_CHECK_ASSERT_X(__VA_ARGS__)(__VA_ARGS__))

// nsx unit test check for integral type equality
#define NSX_CHECK_EQUAL_1(observed,predicted) nsx::allTests().testCheckEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), false, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_EQUAL_2(observed,predicted,expectedFailure) nsx::allTests().testCheckEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), expectedFailure, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_EQUAL_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__, NSX_CHECK_EQUAL_2, NSX_CHECK_EQUAL_1,))
#define NSX_CHECK_EQUAL(...) EXPAND(NSX_CHECK_EQUAL_X(__VA_ARGS__)(__VA_ARGS__))
#define NSX_CHECK_EQ(...) EXPAND(NSX_CHECK_EQUAL(__VA_ARGS__))

// nsx unit test check for integral type inequality
#define NSX_CHECK_NOT_EQUAL_1(observed,predicted) nsx::allTests().testCheckNotEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), false, #observed" != " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_NOT_EQUAL_2(observed,predicted,expectedFailure) nsx::allTests().testCheckNotEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), expectedFailure, #observed" != " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_NOT_EQUAL_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__,NSX_CHECK_NOT_EQUAL_2, NSX_CHECK_NOT_EQUAL_1,))
#define NSX_CHECK_NOT_EQUAL(...) EXPAND(NSX_CHECK_NOT_EQUAL_X(__VA_ARGS__)(__VA_ARGS__))
#define NSX_CHECK_NEQ(...) EXPAND(NSX_CHECK_NOT_EQUAL(__VA_ARGS__))

// nsx unit test check for strict > inequality
#define NSX_CHECK_GREATER_THAN_1(observed,predicted) nsx::allTests().testCheckGreaterThan(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), false, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_GREATER_THAN_2(observed,predicted,expectedFailure) nsx::allTests().testCheckGreaterThan(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), expectedFailure, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_GREATER_THAN_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__, NSX_CHECK_GREATER_THAN_2, NSX_CHECK_GREATER_THAN_1,))
#define NSX_CHECK_GREATER_THAN(...) EXPAND(NSX_CHECK_GREATER_THAN_X(__VA_ARGS__)(__VA_ARGS__))
#define NSX_CHECK_GT(...) EXPAND(NSX_CHECK_GREATER_THAN(__VA_ARGS__))

// nsx unit test check for >= inequality
#define NSX_CHECK_GREATER_THAN_OR_EQUAL_1(observed,predicted) nsx::allTests().testCheckGreaterThanOrEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), false, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_GREATER_THAN_OR_EQUAL_2(observed,predicted,expectedFailure) nsx::allTests().testCheckGreaterThanPrEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), expectedFailure, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_GREATER_THAN_OR_EQUAL_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__, NSX_CHECK_GREATER_THAN_OR_EQUAL_2, NSX_CHECK_GREATER_THAN_OR_EQUAL_1,))
#define NSX_CHECK_GREATER_THAN_OR_EQUAL(...) EXPAND(NSX_CHECK_GREATER_THAN_OR_EQUAL_X(__VA_ARGS__)(__VA_ARGS__))
#define NSX_CHECK_GE(...) EXPAND(NSX_CHECK_GREATER_THAN_OR_EQUAL(__VA_ARGS__))

// nsx unit test check for strict < inequality
#define NSX_CHECK_LOWER_THAN_1(observed,predicted) nsx::allTests().testCheckLowerThan(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), false, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_LOWER_THAN_2(observed,predicted,expectedFailure) nsx::allTests().testCheckLowerThan(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), expectedFailure, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_LOWER_THAN_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__, NSX_CHECK_LOWER_THAN_2, NSX_CHECK_LOWER_THAN_1,))
#define NSX_CHECK_LOWER_THAN(...) EXPAND(NSX_CHECK_LOWER_THAN_X(__VA_ARGS__)(__VA_ARGS__))
#define NSX_CHECK_LT(...) EXPAND(NSX_CHECK_LOWER_THAN(__VA_ARGS__))

// nsx unit test check for <= inequality
#define NSX_CHECK_LOWER_THAN_OR_EQUAL_1(observed,predicted) nsx::allTests().testCheckLowerThanOrEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), false, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_LOWER_THAN_OR_EQUAL_2(observed,predicted,expectedFailure) nsx::allTests().testCheckLowerThanPrEqual(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), expectedFailure, #observed" == " #predicted, __FILE__, __LINE__)
#define NSX_CHECK_LOWER_THAN_OR_EQUAL_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__, NSX_CHECK_LOWER_THAN_OR_EQUAL_2, NSX_CHECK_LOWER_THAN_OR_EQUAL_1,))
#define NSX_CHECK_LOWER_THAN_OR_EQUAL(...) EXPAND(NSX_CHECK_LOWER_THAN_OR_EQUAL_X(__VA_ARGS__)(__VA_ARGS__))
#define NSX_CHECK_LE(...) EXPAND(NSX_CHECK_LOWER_THAN_OR_EQUAL(__VA_ARGS__))

// nsx unit test check for floating type equality
#define NSX_CHECK_CLOSE_1(observed,predicted,epsilon) nsx::allTests().testCheckClose(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), false, "|("#observed" - " #predicted")/"#predicted"| < "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_CLOSE_2(observed,predicted,epsilon,expectedFailure) nsx::allTests().testCheckClose(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), expectedFailure, "|("#observed" - " #predicted")/"#predicted"| < "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_CLOSE_X(...) EXPAND(GET_5TH_ARG(__VA_ARGS__,NSX_CHECK_CLOSE_2, NSX_CHECK_CLOSE_1,))
#define NSX_CHECK_CLOSE(...) EXPAND(NSX_CHECK_CLOSE_X(__VA_ARGS__)(__VA_ARGS__))

// nsx unit test check for floating type inequality
#define NSX_CHECK_NOT_CLOSE_1(observed,predicted,epsilon) nsx::allTests().testCheckNotClose(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), false, "|("#observed" - " #predicted")/"#predicted"| > "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_NOT_CLOSE_2(observed,predicted,epsilon,expectedFailure) nsx::allTests().testCheckNotClose(observed, static_cast< std::remove_reference<decltype(observed)>::type >(predicted), static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), expectedFailure, "|("#observed" - " #predicted")/"#predicted"| > "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_NOT_CLOSE_X(...) EXPAND(GET_5TH_ARG(__VA_ARGS__,NSX_CHECK_NOT_CLOSE_2, NSX_CHECK_NOT_CLOSE_1,))
#define NSX_CHECK_NOT_CLOSE(...) EXPAND(NSX_CHECK_NOT_CLOSE_X(__VA_ARGS__)(__VA_ARGS__))

// nsx unit test check for floating type equality
#define NSX_CHECK_SMALL_1(observed,epsilon) nsx::allTests().testCheckSmall(observed, static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), false, "|"#observed"| < "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_SMALL_2(observed,epsilon,expectedFailure) nsx::allTests().testCheckSmall(observed, static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), expectedFailure, "|"#observed"| < "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_SMALL_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__,NSX_CHECK_SMALL_2, NSX_CHECK_SMALL_1,))
#define NSX_CHECK_SMALL(...) EXPAND(NSX_CHECK_SMALL_X(__VA_ARGS__)(__VA_ARGS__))

// nsx unit test check for floating type inequality
#define NSX_CHECK_NOT_SMALL_1(observed,epsilon) nsx::allTests().testCheckNotSmall(observed, static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), false, "|"#observed"| > "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_NOT_SMALL_2(observed,epsilon,expectedFailure) nsx::allTests().testCheckNotSmall(observed, static_cast< std::remove_reference<decltype(observed)>::type >(epsilon), expectedFailure, "|"#observed"| > "#epsilon"%", __FILE__, __LINE__)
#define NSX_CHECK_NOT_SMALL_X(...) EXPAND(GET_4TH_ARG(__VA_ARGS__,NSX_CHECK_NOT_SMALL_2, NSX_CHECK_NOT_SMALL_1,))
#define NSX_CHECK_NOT_SMALL(...) EXPAND(NSX_CHECK_NOT_SMALL_X(__VA_ARGS__)(__VA_ARGS__))

// nsx failure trigger
#define NSX_FAIL_1(description) nsx::allTests().triggerFailure(false, description,__FILE__,__LINE__)
#define NSX_FAIL_2(expectedFailure, description) nsx::allTests().triggerFailure(expectedFailure, description,__FILE__,__LINE__)
#define NSX_FAIL_X(...) EXPAND(GET_3RD_ARG(__VA_ARGS__,NSX_FAIL_2, NSX_FAIL_1,))
#define NSX_FAIL(...) EXPAND(NSX_FAIL_X(__VA_ARGS__)(__VA_ARGS__))
