#include <nsxlib/Maybe.h>
#include <nsxlib/NSXTest.h>

const double eps = 1e-10;

int main()
{
    nsx::Maybe<int> x(5);

    NSX_CHECK_EQUAL(x.get(), 5);

    int& y = x.get();
    y = 6;

    NSX_CHECK_EQUAL(x.get(), 6);

    // class with deleted default constructor
    class TestClass {
    public:
        TestClass() = delete;
        ~TestClass() {}
    };

    // check that we can create 'nothing' of type TestClass
    nsx::Maybe<TestClass> test;

    // check that we can create maybe of string type (non-trivial destructor)
    nsx::Maybe<std::string> str_nothing;
    nsx::Maybe<std::string> str_something("something");

    return 0;
}
