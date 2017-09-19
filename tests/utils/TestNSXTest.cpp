#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <nsxlib/utils/NSXTest.h>

class MyException : public std::exception {
public:
    using std::exception::exception;
};

int main() {

    // Check assertion

    NSX_CHECK_ASSERT(true);

    NSX_CHECK_ASSERT(false,true);

    // Check integer equality/inequality

    NSX_CHECK_EQUAL(1,1);

    NSX_CHECK_EQUAL(1l,1l);

    NSX_CHECK_EQUAL(1,2,true);

    NSX_CHECK_NOT_EQUAL(1,2);

    // Check floatings equality/inequality within a given tolerance

    NSX_CHECK_CLOSE(1.0,1.1,0.5);

    NSX_CHECK_CLOSE(1.0f,18.3f,0.2f,true);

    NSX_CHECK_NOT_CLOSE(1.0f,1.3f,0.2f);

    NSX_CHECK_NOT_CLOSE(1.0,2.0,10.0,true);

    // Check exception throwed

    NSX_CHECK_EXCEPTION(throw std::runtime_error("I am failing"),std::runtime_error);

    NSX_CHECK_EXCEPTION(throw MyException(),MyException);

    NSX_CHECK_EXCEPTION(throw MyException(),std::exception);

    NSX_CHECK_ANY_EXCEPTION(throw MyException());

    // Check no exception throwed

    NSX_CHECK_NO_EXCEPTION(true);

    NSX_FAIL(true, "I want you to fail !");

    using nsx::all_tests;
    if (all_tests.nTests() != 16)
    {
        std::cout<<"Invalid number of unit tests"<<std::endl;
        std::exit(1);
    }

    if (all_tests.nSuccesses() != 11)
    {
        std::cout<<"Invalid number of successful unit tests"<<std::endl;
        std::exit(1);
    }

    if (all_tests.nSkipped() != 5)
    {
        std::cout<<"Invalid number of skipped unit tests"<<std::endl;
        std::exit(1);
    }

    return 0;
}
