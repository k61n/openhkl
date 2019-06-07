#include <string>

#include <Eigen/Dense>

#include <core/Enums.h>
#include <core/MatrixParser.h>
#include <core/NSXTest.h>

NSX_INIT_TEST

int main()
{
    Eigen::MatrixXi m;
    m.resize(3, 4);
    std::string s("1 2 3 4 5 6 7 8 9 10 11 12");

    nsx::MatrixParser parser;
    parser(nsx::DataOrder::TopLeftColMajor, s, m);

    NSX_CHECK_ASSERT(m(0, 0) == 3);
    NSX_CHECK_ASSERT(m(0, 1) == 6);
    NSX_CHECK_ASSERT(m(0, 2) == 9);
    NSX_CHECK_ASSERT(m(0, 3) == 12);
    NSX_CHECK_ASSERT(m(1, 0) == 2);
    NSX_CHECK_ASSERT(m(1, 1) == 5);
    NSX_CHECK_ASSERT(m(1, 2) == 8);
    NSX_CHECK_ASSERT(m(1, 3) == 11);
    NSX_CHECK_ASSERT(m(2, 0) == 1);
    NSX_CHECK_ASSERT(m(2, 1) == 4);
    NSX_CHECK_ASSERT(m(2, 2) == 7);
    NSX_CHECK_ASSERT(m(2, 3) == 10);

    parser(nsx::DataOrder::TopLeftRowMajor, s, m);
    NSX_CHECK_ASSERT(m(0, 0) == 9);
    NSX_CHECK_ASSERT(m(0, 1) == 10);
    NSX_CHECK_ASSERT(m(0, 2) == 11);
    NSX_CHECK_ASSERT(m(0, 3) == 12);
    NSX_CHECK_ASSERT(m(1, 0) == 5);
    NSX_CHECK_ASSERT(m(1, 1) == 6);
    NSX_CHECK_ASSERT(m(1, 2) == 7);
    NSX_CHECK_ASSERT(m(1, 3) == 8);
    NSX_CHECK_ASSERT(m(2, 0) == 1);
    NSX_CHECK_ASSERT(m(2, 1) == 2);
    NSX_CHECK_ASSERT(m(2, 2) == 3);
    NSX_CHECK_ASSERT(m(2, 3) == 4);

    parser(nsx::DataOrder::TopRightColMajor, s, m);
    NSX_CHECK_ASSERT(m(0, 0) == 12);
    NSX_CHECK_ASSERT(m(0, 1) == 9);
    NSX_CHECK_ASSERT(m(0, 2) == 6);
    NSX_CHECK_ASSERT(m(0, 3) == 3);
    NSX_CHECK_ASSERT(m(1, 0) == 11);
    NSX_CHECK_ASSERT(m(1, 1) == 8);
    NSX_CHECK_ASSERT(m(1, 2) == 5);
    NSX_CHECK_ASSERT(m(1, 3) == 2);
    NSX_CHECK_ASSERT(m(2, 0) == 10);
    NSX_CHECK_ASSERT(m(2, 1) == 7);
    NSX_CHECK_ASSERT(m(2, 2) == 4);
    NSX_CHECK_ASSERT(m(2, 3) == 1);

    parser(nsx::DataOrder::TopRightRowMajor, s, m);
    NSX_CHECK_ASSERT(m(0, 0) == 12);
    NSX_CHECK_ASSERT(m(0, 1) == 11);
    NSX_CHECK_ASSERT(m(0, 2) == 10);
    NSX_CHECK_ASSERT(m(0, 3) == 9);
    NSX_CHECK_ASSERT(m(1, 0) == 8);
    NSX_CHECK_ASSERT(m(1, 1) == 7);
    NSX_CHECK_ASSERT(m(1, 2) == 6);
    NSX_CHECK_ASSERT(m(1, 3) == 5);
    NSX_CHECK_ASSERT(m(2, 0) == 4);
    NSX_CHECK_ASSERT(m(2, 1) == 3);
    NSX_CHECK_ASSERT(m(2, 2) == 2);
    NSX_CHECK_ASSERT(m(2, 3) == 1);

    parser(nsx::DataOrder::BottomLeftColMajor, s, m);
    NSX_CHECK_ASSERT(m(0, 0) == 1);
    NSX_CHECK_ASSERT(m(0, 1) == 4);
    NSX_CHECK_ASSERT(m(0, 2) == 7);
    NSX_CHECK_ASSERT(m(0, 3) == 10);
    NSX_CHECK_ASSERT(m(1, 0) == 2);
    NSX_CHECK_ASSERT(m(1, 1) == 5);
    NSX_CHECK_ASSERT(m(1, 2) == 8);
    NSX_CHECK_ASSERT(m(1, 3) == 11);
    NSX_CHECK_ASSERT(m(2, 0) == 3);
    NSX_CHECK_ASSERT(m(2, 1) == 6);
    NSX_CHECK_ASSERT(m(2, 2) == 9);
    NSX_CHECK_ASSERT(m(2, 3) == 12);

    parser(nsx::DataOrder::BottomLeftRowMajor, s, m);
    NSX_CHECK_ASSERT(m(0, 0) == 1);
    NSX_CHECK_ASSERT(m(0, 1) == 2);
    NSX_CHECK_ASSERT(m(0, 2) == 3);
    NSX_CHECK_ASSERT(m(0, 3) == 4);
    NSX_CHECK_ASSERT(m(1, 0) == 5);
    NSX_CHECK_ASSERT(m(1, 1) == 6);
    NSX_CHECK_ASSERT(m(1, 2) == 7);
    NSX_CHECK_ASSERT(m(1, 3) == 8);
    NSX_CHECK_ASSERT(m(2, 0) == 9);
    NSX_CHECK_ASSERT(m(2, 1) == 10);
    NSX_CHECK_ASSERT(m(2, 2) == 11);
    NSX_CHECK_ASSERT(m(2, 3) == 12);

    parser(nsx::DataOrder::BottomRightColMajor, s, m);
    NSX_CHECK_ASSERT(m(0, 0) == 10);
    NSX_CHECK_ASSERT(m(0, 1) == 7);
    NSX_CHECK_ASSERT(m(0, 2) == 4);
    NSX_CHECK_ASSERT(m(0, 3) == 1);
    NSX_CHECK_ASSERT(m(1, 0) == 11);
    NSX_CHECK_ASSERT(m(1, 1) == 8);
    NSX_CHECK_ASSERT(m(1, 2) == 5);
    NSX_CHECK_ASSERT(m(1, 3) == 2);
    NSX_CHECK_ASSERT(m(2, 0) == 12);
    NSX_CHECK_ASSERT(m(2, 1) == 9);
    NSX_CHECK_ASSERT(m(2, 2) == 6);
    NSX_CHECK_ASSERT(m(2, 3) == 3);

    parser(nsx::DataOrder::BottomRightRowMajor, s, m);
    NSX_CHECK_ASSERT(m(0, 0) == 4);
    NSX_CHECK_ASSERT(m(0, 1) == 3);
    NSX_CHECK_ASSERT(m(0, 2) == 2);
    NSX_CHECK_ASSERT(m(0, 3) == 1);
    NSX_CHECK_ASSERT(m(1, 0) == 8);
    NSX_CHECK_ASSERT(m(1, 1) == 7);
    NSX_CHECK_ASSERT(m(1, 2) == 6);
    NSX_CHECK_ASSERT(m(1, 3) == 5);
    NSX_CHECK_ASSERT(m(2, 0) == 12);
    NSX_CHECK_ASSERT(m(2, 1) == 11);
    NSX_CHECK_ASSERT(m(2, 2) == 10);
    NSX_CHECK_ASSERT(m(2, 3) == 9);

    return 0;
}
