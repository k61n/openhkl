#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <core/NSXTest.h>

NSX_INIT_TEST

class MyException : public std::exception {
public:
  using std::exception::exception;
};

int main() {

  NSX_INIT_TEST

  // Check assertion

  NSX_CHECK_ASSERT(true == true);

  NSX_CHECK_ASSERT(false == true, true);

  // Check integer equality/inequality

  NSX_CHECK_EQUAL(1, 1);

  NSX_CHECK_EQ(1, 1);

  NSX_CHECK_EQUAL(1l, 1l);

  NSX_CHECK_EQUAL(1, 2, true);

  NSX_CHECK_NOT_EQUAL(1, 2);

  NSX_CHECK_NEQ(1, 2);

  NSX_CHECK_GREATER_THAN(2l, 1l);

  NSX_CHECK_GT(2l, 1l);

  NSX_CHECK_GREATER_THAN_OR_EQUAL(2l, 1l);

  NSX_CHECK_GE(2l, 1l);

  NSX_CHECK_LOWER_THAN(1l, 2l);

  NSX_CHECK_LT(1l, 2l);

  NSX_CHECK_LOWER_THAN_OR_EQUAL(1l, 2l);

  NSX_CHECK_LE(1l, 2l);

  // Check floatings equality/inequality within a given tolerance

  // 1 and 1.1 are close at 15%
  NSX_CHECK_CLOSE(1.0, 1.1, 15.0);

  // 1.0 and 1.1 and not close at8%
  NSX_CHECK_CLOSE(1.0f, 1.1f, 8.0f, true);

  // 1.0 and 1.3 are not close at 10%
  NSX_CHECK_NOT_CLOSE(1.0f, 1.3f, 10.0f);

  // 1.0 and 2.0 close at 60%
  NSX_CHECK_NOT_CLOSE(1.0, 2.0, 60.0, true);

  // Check exception throwed

  NSX_CHECK_THROW(throw std::runtime_error("I am failing"), std::runtime_error);

  NSX_CHECK_THROW(throw MyException(), MyException);

  NSX_CHECK_THROW(throw MyException(), std::exception);

  NSX_CHECK_THROW_ANY(throw MyException());

  // Check no exception throwed

  NSX_CHECK_NO_THROW(true);

  NSX_FAIL(true, "I want you to fail !");

  if (all_tests.nTests() != 26) {
    std::cout << "Invalid number of unit tests" << std::endl;
    std::exit(1);
  }

  if (all_tests.nSuccesses() != 21) {
    std::cout << "Invalid number of successful unit tests" << std::endl;
    std::exit(1);
  }

  if (all_tests.nSkipped() != 5) {
    std::cout << "Invalid number of skipped unit tests" << std::endl;
    std::exit(1);
  }

  return 0;
}
