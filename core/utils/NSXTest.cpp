//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/NSXTest.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cstdlib>

#include "NSXTest.h"

namespace nsx {

NSXTest::NSXTest() : _n_failures(0), _n_successes(0), _n_skipped(0) {}

NSXTest::~NSXTest()
{
    std::cout << "------------------------------------------------------\n";
    std::cout << "all test completed: " << _n_successes << " successes -- " << _n_failures
              << " failures -- " << _n_skipped << " skipped.";
    std::cout << std::endl;

    if (_n_failures > 0) {
        std::exit(1);
    }
}

void NSXTest::testCheckAssert(
    bool condition, bool expectedFailure, const std::string& description,
    const std::string& filename, int lineno)
{

    if (condition) {
        ++_n_successes;
    } else {
        if (expectedFailure) {
            ++_n_skipped;
        } else {
            ++_n_failures;
            std::cout << filename << "(" << lineno << "): TEST `" << description << "' FAILED"
                      << std::endl;
        }
    }
}

void NSXTest::triggerFailure(
    bool expectedFailure, const std::string& description, const std::string& filename, int lineno)
{

    if (expectedFailure) {
        ++_n_skipped;
    } else {
        std::cout << filename << "(" << lineno << "): TEST `" << description << "' FAILED"
                  << std::endl;
        ++_n_failures;
    }
}

void NSXTest::triggerSuccess()
{
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

int NSXTest::nSkipped() const
{
    return _n_skipped;
}

int NSXTest::nTests() const
{
    return _n_failures + _n_successes + _n_skipped;
}

} // end namespace nsx
