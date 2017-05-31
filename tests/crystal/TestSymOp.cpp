#define BOOST_TEST_MODULE "Test Symmetry Operator"
#define BOOST_TEST_DYN_LINK

#include <stdexcept>
#include <string>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/SymOp.h>

BOOST_AUTO_TEST_CASE(Test_SymOp)
{

    nsx::SymOp op1("x,y,z");
    BOOST_CHECK_EQUAL(op1.getAxisOrder(),1);

    nsx::SymOp op2("-x+1/2,y,-z");
    BOOST_CHECK_EQUAL(op2.getAxisOrder(),2);

    nsx::SymOp op3("y,-x-y,z");
    BOOST_CHECK_EQUAL(op3.getAxisOrder(),3);

    nsx::SymOp op4("y,-x,z");
    BOOST_CHECK_EQUAL(op4.getAxisOrder(),4);

    nsx::SymOp op6("x+y,-x,z");
    BOOST_CHECK_EQUAL(op6.getAxisOrder(),6);

    nsx::SymOp op7("-x,y,z");
    BOOST_CHECK_EQUAL(op7.getAxisOrder(),-2);

    nsx::SymOp op8("-y,x+y,-z");
    BOOST_CHECK_EQUAL(op8.getAxisOrder(),-3);

    nsx::SymOp op9("-y,x,-z");
    BOOST_CHECK_EQUAL(op9.getAxisOrder(),-4);

    nsx::SymOp op10("-x-y,x,-z");
    BOOST_CHECK_EQUAL(op10.getAxisOrder(),-6);

    nsx::SymOp op11("x-y,x+2y,-3z");
    BOOST_CHECK_THROW(op11.getAxisOrder(),std::runtime_error);

}
