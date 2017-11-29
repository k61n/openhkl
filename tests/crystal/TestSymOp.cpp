#include <stdexcept>
#include <string>

#include <nsxlib/NSXTest.h>
#include <nsxlib/SymOp.h>

int main()
{
    nsx::SymOp op1("x,y,z");
    NSX_CHECK_EQUAL(op1.getAxisOrder(),1);

    nsx::SymOp op2("-x+1/2,y,-z");
    NSX_CHECK_EQUAL(op2.getAxisOrder(),2);

    nsx::SymOp op3("y,-x-y,z");
    NSX_CHECK_EQUAL(op3.getAxisOrder(),3);

    nsx::SymOp op4("y,-x,z");
    NSX_CHECK_EQUAL(op4.getAxisOrder(),4);

    nsx::SymOp op6("x+y,-x,z");
    NSX_CHECK_EQUAL(op6.getAxisOrder(),6);

    nsx::SymOp op7("-x,y,z");
    NSX_CHECK_EQUAL(op7.getAxisOrder(),-2);

    nsx::SymOp op8("-y,x+y,-z");
    NSX_CHECK_EQUAL(op8.getAxisOrder(),-3);

    nsx::SymOp op9("-y,x,-z");
    NSX_CHECK_EQUAL(op9.getAxisOrder(),-4);

    nsx::SymOp op10("-x-y,x,-z");
    NSX_CHECK_EQUAL(op10.getAxisOrder(),-6);

    nsx::SymOp op11("x-y,x+2y,-3z");
    NSX_CHECK_THROW(op11.getAxisOrder(),std::runtime_error);

    return 0;
}
