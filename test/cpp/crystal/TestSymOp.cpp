#include "test/cpp/catch.hpp"
#include <stdexcept>
#include <string>

#include "core/crystal/SymOp.h"

TEST_CASE("test/crystal/TestSymOp.cpp", "")
{

    nsx::SymOp op1("x,y,z");
    CHECK(op1.getAxisOrder() == 1);

    nsx::SymOp op2("-x+1/2,y,-z");
    CHECK(op2.getAxisOrder() == 2);

    nsx::SymOp op3("y,-x-y,z");
    CHECK(op3.getAxisOrder() == 3);

    nsx::SymOp op4("y,-x,z");
    CHECK(op4.getAxisOrder() == 4);

    nsx::SymOp op6("x+y,-x,z");
    CHECK(op6.getAxisOrder() == 6);

    nsx::SymOp op7("-x,y,z");
    CHECK(op7.getAxisOrder() == -2);

    nsx::SymOp op8("-y,x+y,-z");
    CHECK(op8.getAxisOrder() == -3);

    nsx::SymOp op9("-y,x,-z");
    CHECK(op9.getAxisOrder() == -4);

    nsx::SymOp op10("-x-y,x,-z");
    CHECK(op10.getAxisOrder() == -6);

    nsx::SymOp op11("x-y,x+2y,-3z");
    CHECK_THROWS_AS(op11.getAxisOrder(), std::runtime_error);
}
