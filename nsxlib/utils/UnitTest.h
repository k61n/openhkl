#pragma once

#include <string>

namespace {

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

private:

    int _n_successes;

    int _n_failures;

};

} // end namespace nsx
