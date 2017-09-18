#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <nsxlib/utils/NSXTest.h>

class MyException : public std::exception {
public:
    using std::exception::exception;
};

int main() {

    using nsx::all_tests;

    int n_tests = 0;

    // Check assertion

    NSX_TEST_CHECK_ASSERT(true);
    if (all_tests.nSuccesses() != 1 || all_tests.nFailures() != 0 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    // Check integer equality/inequality

    NSX_TEST_CHECK_EQUAL(1,1);
    if (all_tests.nSuccesses() != 2 || all_tests.nFailures() != 0 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_EQUAL(1l,1l);
    if (all_tests.nSuccesses() != 3 || all_tests.nFailures() != 0 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_EQUAL(1,2);
    if (all_tests.nSuccesses() != 3 || all_tests.nFailures() != 1 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_NOT_EQUAL(1,2);
    if (all_tests.nSuccesses() != 4 || all_tests.nFailures() != 1 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    // Check floatings equality/inequality within a given tolerance

    NSX_TEST_CHECK_CLOSE(1.0,1.1,0.5);
    if (all_tests.nSuccesses() != 5 || all_tests.nFailures() != 1 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_CLOSE(1.0f,18.3f,0.2f);
    if (all_tests.nSuccesses() != 5 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_NOT_CLOSE(1.0f,1.3f,0.2f);
    if (all_tests.nSuccesses() != 6 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_NOT_CLOSE(1.0,2.0,0.1);
    if (all_tests.nSuccesses() != 7 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    // Check exception throwed

    NSX_TEST_CHECK_EXCEPTION(throw std::runtime_error("I am failing"),std::runtime_error);
    if (all_tests.nSuccesses() != 8 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_EXCEPTION(throw MyException(),MyException);
    if (all_tests.nSuccesses() != 9 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_EXCEPTION(throw MyException(),std::exception);
    if (all_tests.nSuccesses() != 10 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_ANY_EXCEPTION(throw MyException());
    if (all_tests.nSuccesses() != 11 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    // Check no exception throwed

    NSX_TEST_CHECK_NO_EXCEPTION(true);
    if (all_tests.nSuccesses() != 12 || all_tests.nFailures() != 2 || all_tests.nTests() != ++n_tests) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    // Reset the unit tests to not make the actual unit test failing (should NEVER be user in other nsxlib unit tests)
    all_tests.reset();

    return 0;
}
