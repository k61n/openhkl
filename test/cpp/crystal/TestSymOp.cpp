//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/crystal/TestSymOp.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <stdexcept>
#include <string>

#include "tables/crystal/SymOp.h"

TEST_CASE("test/crystal/TestSymOp.cpp", "")
{
    ohkl::SymOp op1("x,y,z");
    CHECK(op1.getAxisOrder() == 1);

    ohkl::SymOp op2("-x+1/2,y,-z");
    CHECK(op2.getAxisOrder() == 2);

    ohkl::SymOp op3("y,-x-y,z");
    CHECK(op3.getAxisOrder() == 3);

    ohkl::SymOp op4("y,-x,z");
    CHECK(op4.getAxisOrder() == 4);

    ohkl::SymOp op6("x+y,-x,z");
    CHECK(op6.getAxisOrder() == 6);

    ohkl::SymOp op7("-x,y,z");
    CHECK(op7.getAxisOrder() == -2);

    ohkl::SymOp op8("-y,x+y,-z");
    CHECK(op8.getAxisOrder() == -3);

    ohkl::SymOp op9("-y,x,-z");
    CHECK(op9.getAxisOrder() == -4);

    ohkl::SymOp op10("-x-y,x,-z");
    CHECK(op10.getAxisOrder() == -6);

    ohkl::SymOp op11("x-y,x+2y,-3z");
    CHECK_THROWS_AS(op11.getAxisOrder(), std::runtime_error);
}
