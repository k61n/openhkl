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

    NSX_TEST_CHECK_ASSERT(true);
    if (all_tests.nSuccesses() != 1 || all_tests.nFailures() != 0 || all_tests.nTests() != 1) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_EQUAL(1,1);
    if (all_tests.nSuccesses() != 2 || all_tests.nFailures() != 0 || all_tests.nTests() != 2) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_EQUAL(1l,1l);
    if (all_tests.nSuccesses() != 3 || all_tests.nFailures() != 0 || all_tests.nTests() != 3) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_EQUAL(1,2);
    if (all_tests.nSuccesses() != 3 || all_tests.nFailures() != 1 || all_tests.nTests() != 4) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_CLOSE(1.0,1.1,0.5);
    if (all_tests.nSuccesses() != 4 || all_tests.nFailures() != 1 || all_tests.nTests() != 5) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_CLOSE(1.0,2.0,0.1);
    if (all_tests.nSuccesses() != 4 || all_tests.nFailures() != 2 || all_tests.nTests() != 6) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_CLOSE(1.0f,1.3f,0.2f);
    if (all_tests.nSuccesses() != 4 || all_tests.nFailures() != 3 || all_tests.nTests() != 7) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_THROW(throw std::runtime_error("I am failing"),std::runtime_error);
    if (all_tests.nSuccesses() != 5 || all_tests.nFailures() != 3 || all_tests.nTests() != 8) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_THROW(true,std::runtime_error);
    if (all_tests.nSuccesses() != 5 || all_tests.nFailures() != 4 || all_tests.nTests() != 9) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_NOT_THROW(throw std::runtime_error("I am failing"));
    if (all_tests.nSuccesses() != 5 || all_tests.nFailures() != 5 || all_tests.nTests() != 10) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_NOT_THROW(true);
    if (all_tests.nSuccesses() != 6 || all_tests.nFailures() != 5 || all_tests.nTests() != 11) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_THROW(throw MyException(),MyException);
    if (all_tests.nSuccesses() != 5 || all_tests.nFailures() != 3 || all_tests.nTests() != 8) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    NSX_TEST_CHECK_THROW(throw MyException(),std::runtime_error);
    if (all_tests.nSuccesses() != 5 || all_tests.nFailures() != 3 || all_tests.nTests() != 8) {
        std::cout<<"Error when performing nsx unit test framework"<<std::endl;
        return 1;
    }

    return 0;
}
