#define BOOST_TEST_MODULE "Test Some"
#define BOOST_TEST_DYN_LINK

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <nsxlib/utils/Some.h>

const double eps = 1e-10;

int run_test()
{
    // Test empty
    nsx::Some<> empty_a;
    BOOST_CHECK(empty_a.empty());

    // Test constructs

    nsx::Some<> a0 = true;
    nsx::Some<> a1 = 'A';
    nsx::Some<> a2 = 6;
    nsx::Some<> a3 = 3.14;
    nsx::Some<> a4 = "hello,";
    nsx::Some<> a5 = std::vector<int>{6, 5, 4, 3, 2, 1, 0};
    nsx::Some<> a6 = 2.71;
    nsx::Some<> a7 = std::string("world!");

    BOOST_CHECK(a0.cast<bool>());
    BOOST_CHECK_EQUAL(a1.cast<char>(),'A');
    BOOST_CHECK_EQUAL(a2.cast<int>(),6);
    BOOST_CHECK_CLOSE(a3.cast<double>(),3.14,eps);
    BOOST_CHECK_EQUAL(std::strcmp(a4.cast<const char*>(),"hello,"),0);
    BOOST_CHECK(a5.cast<std::vector<int>>()==std::vector<int>({6, 5, 4, 3, 2, 1, 0}));
    BOOST_CHECK_CLOSE(a6.cast<double>(),2.71,eps);
    BOOST_CHECK_EQUAL(a7.cast<std::string>().compare("world!"),0);

    // Test copy

    nsx::Some<> c0 = a0;
    nsx::Some<> c1 = a1;
    nsx::Some<> c2 = a2;
    nsx::Some<> c3 = a3;
    nsx::Some<> c4{a4};
    nsx::Some<> c5{a5};
    nsx::Some<> c6{a6};
    nsx::Some<> c7{a7};

    BOOST_CHECK(c0.cast<bool>());
    BOOST_CHECK_EQUAL(c1.cast<char>(),'A');
    BOOST_CHECK_EQUAL(c2.cast<int>(),6);
    BOOST_CHECK_CLOSE(c3.cast<double>(),3.14,eps);
    BOOST_CHECK_EQUAL(std::strcmp(c4.cast<const char*>(),"hello,"),0);
    BOOST_CHECK(c5.cast<std::vector<int>>()==std::vector<int>({6, 5, 4, 3, 2, 1, 0}));
    BOOST_CHECK_CLOSE(c6.cast<double>(),2.71,eps);
    BOOST_CHECK_EQUAL(c7.cast<std::string>().compare("world!"),0);

    // Test move

    nsx::Some<> m0 = std::move(c0);
    nsx::Some<> m1 = std::move(c1);
    nsx::Some<> m2 = std::move(c2);
    nsx::Some<> m3 = std::move(c3);
    nsx::Some<> m4{std::move(c4)};
    nsx::Some<> m5{std::move(c5)};
    nsx::Some<> m6{std::move(c6)};
    nsx::Some<> m7{std::move(c7)};

    BOOST_CHECK(m0.cast<bool>());
    BOOST_CHECK_EQUAL(m1.cast<char>(),'A');
    BOOST_CHECK_EQUAL(m2.cast<int>(),6);
    BOOST_CHECK_CLOSE(m3.cast<double>(),3.14,eps);
    BOOST_CHECK_EQUAL(std::strcmp(m4.cast<const char*>(),"hello,"),0);
    BOOST_CHECK(m5.cast<std::vector<int>>()==std::vector<int>({6, 5, 4, 3, 2, 1, 0}));
    BOOST_CHECK_CLOSE(m6.cast<double>(),2.71,eps);
    BOOST_CHECK_EQUAL(m7.cast<std::string>().compare("world!"),0);

    // Test container

    std::vector<nsx::Some<>> a(8);
    a[0] = true;
    a[1] = 'A';
    a[2] = 6;
    a[3] = 3.14;
    a[4] = "hello,";
    a[5] = std::vector<int>{6, 5, 4, 3, 2, 1, 0};
    a[6] = 2.71;
    a[7] = std::string("world!");

    BOOST_CHECK(a[0].cast<bool>());
    BOOST_CHECK_EQUAL(a[1].cast<char>(),'A');
    BOOST_CHECK_EQUAL(a[2].cast<int>(),6);
    BOOST_CHECK_CLOSE(a[3].cast<double>(),3.14,eps);
    BOOST_CHECK_EQUAL(std::strcmp(a[4].cast<const char*>(),"hello,"),0);
    BOOST_CHECK(a[5].cast<std::vector<int>>()==std::vector<int>({6, 5, 4, 3, 2, 1, 0}));
    BOOST_CHECK_CLOSE(a[6].cast<double>(),2.71,eps);
    BOOST_CHECK_EQUAL(a[7].cast<std::string>().compare("world!"),0);

    a[0] = m0;
    a[1] = m1;
    a[2] = m2;
    a[3] = m3;
    a[4] = m4;
    a[5] = m5;
    a[6] = m6;
    a[7] = m7;

    BOOST_CHECK(a[0].cast<bool>());
    BOOST_CHECK_EQUAL(a[1].cast<char>(),'A');
    BOOST_CHECK_EQUAL(a[2].cast<int>(),6);
    BOOST_CHECK_CLOSE(a[3].cast<double>(),3.14,eps);
    BOOST_CHECK_EQUAL(std::strcmp(a[4].cast<const char*>(),"hello,"),0);
    BOOST_CHECK(a[5].cast<std::vector<int>>()==std::vector<int>({6, 5, 4, 3, 2, 1, 0}));
    BOOST_CHECK_CLOSE(a[6].cast<double>(),2.71,eps);
    BOOST_CHECK_EQUAL(a[7].cast<std::string>().compare("world!"),0);

    a[0] = std::move(m0);
    a[1] = std::move(m1);
    a[2] = std::move(m2);
    a[3] = std::move(m3);
    a[4] = std::move(m4);
    a[5] = std::move(m5);
    a[6] = std::move(m6);
    a[7] = std::move(m7);

    BOOST_CHECK(a[0].cast<bool>());
    BOOST_CHECK_EQUAL(a[1].cast<char>(),'A');
    BOOST_CHECK_EQUAL(a[2].cast<int>(),6);
    BOOST_CHECK_CLOSE(a[3].cast<double>(),3.14,eps);
    BOOST_CHECK_EQUAL(std::strcmp(a[4].cast<const char*>(),"hello,"),0);
    BOOST_CHECK(a[5].cast<std::vector<int>>()==std::vector<int>({6, 5, 4, 3, 2, 1, 0}));
    BOOST_CHECK_CLOSE(a[6].cast<double>(),2.71,eps);
    BOOST_CHECK_EQUAL(a[7].cast<std::string>().compare("world!"),0);

    // Test is

    auto is_double = [](const nsx::Some<>& s) -> double { return s.is<double>(); };
    BOOST_CHECK_EQUAL(std::count_if(a.begin(), a.end(), is_double),2);

    a0 = a[0];
    a1 = a[1];
    a2 = a[2];
    a3 = a[3];
    a4 = a[4];
    a5 = a[5];
    a6 = a[6];
    a7 = a[7];

    BOOST_CHECK(!a[0].is<long>());
    BOOST_CHECK(a[0].is<bool>());
    BOOST_CHECK(!a[1].is<long>());
    BOOST_CHECK(a[1].is<char>());
    BOOST_CHECK(!a[2].is<long>());
    BOOST_CHECK(a[2].is<int>());
    BOOST_CHECK(!a[3].is<long>());
    BOOST_CHECK(a[3].is<double>());
    BOOST_CHECK(!a[4].is<long>());
    BOOST_CHECK(a[4].is<const char*>());
    BOOST_CHECK(!a[5].is<long>());
    BOOST_CHECK(a[5].is<std::vector<int>>());
    BOOST_CHECK(!a[6].is<long>());
    BOOST_CHECK(a[6].is<double>());
    BOOST_CHECK(!a[7].is<long>());
    BOOST_CHECK(a[7].is<std::string>());

    // Test swap

    swap(a5,a7);
    BOOST_CHECK(a5.is<std::string>());
    BOOST_CHECK(a7.is<std::vector<int>>());

    swap(a5,a7);
    BOOST_CHECK(a5.is<std::vector<int>>());
    BOOST_CHECK(a7.is<std::string>());

    // Test lvalue references

    bool& l0(a0);
    char& l1(a1);
    int& l2(a2);
    double& l3(a3);
    const char*& l4 = a4;
    std::vector<int>& l5 = a5;
    double& l6 = a6;
    std::string& l7 = a7;

    l0 = false;
    l1 = 'B';
    l2 = 8;
    l3 = -3.14;
    l4 = "Hello,";
    l5[2] = -2;
    l6 = -2.71;
    l7 += " ...";

    BOOST_CHECK(!a0.cast<bool>());
    BOOST_CHECK_EQUAL(a1.cast<char>(),'B');
    BOOST_CHECK_EQUAL(a2.cast<int>(),8);
    BOOST_CHECK_CLOSE(a3.cast<double>(),-3.14,eps);
    BOOST_CHECK_EQUAL(std::strcmp(a4.cast<const char*>(),"Hello,"),0);
    BOOST_CHECK(a5.cast<std::vector<int>>()==std::vector<int>({6, 5, -2, 3, 2, 1, 0}));
    BOOST_CHECK_CLOSE(a6.cast<double>(),-2.71,eps);
    BOOST_CHECK_EQUAL(a7.cast<std::string>().compare("world! ..."),0);

    a5.cast<std::vector<int>>()[4] = -2;
    a7.cast<std::string>()[0] = 'W';

    BOOST_CHECK(l5==std::vector<int>({6, 5, -2, 3, -2, 1, 0}));
    BOOST_CHECK_EQUAL(l7.compare("World! ..."),0);

    // Test rvalue references

    bool&& r0(std::move(a0));
    char&& r1(std::move(a1));
    int&& r2(std::move(a2));
    double&& r3(std::move(a3));
    const char*&& r4 = std::move(a4);
    double&& r6 = std::move(a6);
    std::vector<int>&& r5 = std::move(a5);
    std::string&& r7 = std::move(a7);

    r0 = true;
    r1 = 'C';
    r2 = 12;
    r3 = 3.14159;
    r4 = "HEllo,";
    r5[3] = -4;
    r6 = 2.71828;
    r7 += " !!!";

    BOOST_CHECK(r0);
    BOOST_CHECK_EQUAL(r1,'C');
    BOOST_CHECK_EQUAL(r2,12);
    BOOST_CHECK_CLOSE(r3,3.14159,eps);
    BOOST_CHECK_EQUAL(std::strcmp(r4,"HEllo,"),0);
    BOOST_CHECK(r5==std::vector<int>({6, 5, -2, -4,-2, 1, 0}));
    BOOST_CHECK_CLOSE(r6,2.71828,eps);
    BOOST_CHECK_EQUAL(r7.compare("World! ... !!!"),0);

    std::move(a5).cast<std::vector<int>>()[5] = -4;
    std::move(a7).cast<std::string>()[1] = 'O';
    BOOST_CHECK(r5==std::vector<int>({6, 5, -2, -4,-2, -4, 0}));
    BOOST_CHECK_EQUAL(r7.compare("WOrld! ... !!!"),0);

    // Test new value

    bool             x0{a0};
    char             x1{a1};
    int              x2{a2};
    double           x3{a3};
    const char*      x4 = a4;
    std::vector<int> x5 = a5;
    double           x6 = a6;
    std::string      x7 = a7;

    BOOST_CHECK(x0);
    BOOST_CHECK_EQUAL(x1,'C');
    BOOST_CHECK_EQUAL(x2,12);
    BOOST_CHECK_CLOSE(x3,3.14159,eps);
    BOOST_CHECK_EQUAL(std::strcmp(x4,"HEllo,"),0);
    BOOST_CHECK(x5==std::vector<int>({6, 5, -2, -4,-2, -4, 0}));
    BOOST_CHECK_CLOSE(x6,2.71828,eps);
    BOOST_CHECK_EQUAL(x7.compare("WOrld! ... !!!"),0);

    // Test new value through move semantics

    bool             y0{std::move(a0)};
    char             y1{std::move(a1)};
    int              y2{std::move(a2)};
    double           y3{std::move(a3)};
    const char*      y4 = std::move(a4);
    std::vector<int> y5 = std::move(a5);
    double           y6 = std::move(a6);
    std::string      y7 = std::move(a7);

    BOOST_CHECK(y0);
    BOOST_CHECK_EQUAL(y1,'C');
    BOOST_CHECK_EQUAL(y2,12);
    BOOST_CHECK_CLOSE(y3,3.14159,eps);
    BOOST_CHECK_EQUAL(std::strcmp(y4,"HEllo,"),0);
    BOOST_CHECK(y5==std::vector<int>({6, 5, -2, -4,-2, -4, 0}));
    BOOST_CHECK_CLOSE(y6,2.71828,eps);
    BOOST_CHECK_EQUAL(y7.compare("WOrld! ... !!!"),0);

    // Test check throw

    struct E
    {
        E() { }
        E(E const&) { throw std::exception(); }
    };

    nsx::Some<> e;

    BOOST_CHECK_NO_THROW(e = 4);
    BOOST_CHECK_THROW(e=E(),std::exception);

    BOOST_CHECK_NO_THROW(e = std::vector<int>({1, 2, 3}));
    BOOST_CHECK_THROW(e=E(),std::exception);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Some)
{
    BOOST_CHECK(run_test() == 0);
}
