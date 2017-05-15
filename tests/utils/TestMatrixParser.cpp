#define BOOST_TEST_MODULE "Test Matrix Parser"
#define BOOST_TEST_DYN_LINK

#include <iostream>
#include <string>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/utils/Enums.h>
#include <nsxlib/utils/MatrixParser.h>

using nsx::Utils::MatrixParser;
using nsx::Instrument::DataOrder;

BOOST_AUTO_TEST_CASE(Test_MatrixParser)
{
    Eigen::MatrixXi m;
    m.resize(3,4);
    std::string s("1 2 3 4 5 6 7 8 9 10 11 12");

    MatrixParser parser;
    parser(DataOrder::TopLeftColMajor,s,m);

    BOOST_CHECK(m(0,0)==3);BOOST_CHECK(m(0,1)==6);BOOST_CHECK(m(0,2)==9);BOOST_CHECK(m(0,3)==12);
    BOOST_CHECK(m(1,0)==2);BOOST_CHECK(m(1,1)==5);BOOST_CHECK(m(1,2)==8);BOOST_CHECK(m(1,3)==11);
    BOOST_CHECK(m(2,0)==1);BOOST_CHECK(m(2,1)==4);BOOST_CHECK(m(2,2)==7);BOOST_CHECK(m(2,3)==10);

    parser(DataOrder::TopLeftRowMajor,s,m);
    BOOST_CHECK(m(0,0)==9);BOOST_CHECK(m(0,1)==10);BOOST_CHECK(m(0,2)==11);BOOST_CHECK(m(0,3)==12);
    BOOST_CHECK(m(1,0)==5);BOOST_CHECK(m(1,1)==6);BOOST_CHECK(m(1,2)==7);BOOST_CHECK(m(1,3)==8);
    BOOST_CHECK(m(2,0)==1);BOOST_CHECK(m(2,1)==2);BOOST_CHECK(m(2,2)==3);BOOST_CHECK(m(2,3)==4);

    parser(DataOrder::TopRightColMajor,s,m);
    BOOST_CHECK(m(0,0)==12);BOOST_CHECK(m(0,1)==9);BOOST_CHECK(m(0,2)==6);BOOST_CHECK(m(0,3)==3);
    BOOST_CHECK(m(1,0)==11);BOOST_CHECK(m(1,1)==8);BOOST_CHECK(m(1,2)==5);BOOST_CHECK(m(1,3)==2);
    BOOST_CHECK(m(2,0)==10);BOOST_CHECK(m(2,1)==7);BOOST_CHECK(m(2,2)==4);BOOST_CHECK(m(2,3)==1);

    parser(DataOrder::TopRightRowMajor,s,m);
    BOOST_CHECK(m(0,0)==12);BOOST_CHECK(m(0,1)==11);BOOST_CHECK(m(0,2)==10);BOOST_CHECK(m(0,3)==9);
    BOOST_CHECK(m(1,0)==8);BOOST_CHECK(m(1,1)==7);BOOST_CHECK(m(1,2)==6);BOOST_CHECK(m(1,3)==5);
    BOOST_CHECK(m(2,0)==4);BOOST_CHECK(m(2,1)==3);BOOST_CHECK(m(2,2)==2);BOOST_CHECK(m(2,3)==1);

    parser(DataOrder::BottomLeftColMajor,s,m);
    BOOST_CHECK(m(0,0)==1);BOOST_CHECK(m(0,1)==4);BOOST_CHECK(m(0,2)==7);BOOST_CHECK(m(0,3)==10);
    BOOST_CHECK(m(1,0)==2);BOOST_CHECK(m(1,1)==5);BOOST_CHECK(m(1,2)==8);BOOST_CHECK(m(1,3)==11);
    BOOST_CHECK(m(2,0)==3);BOOST_CHECK(m(2,1)==6);BOOST_CHECK(m(2,2)==9);BOOST_CHECK(m(2,3)==12);

    parser(DataOrder::BottomLeftRowMajor,s,m);
    BOOST_CHECK(m(0,0)==1);BOOST_CHECK(m(0,1)==2);BOOST_CHECK(m(0,2)==3);BOOST_CHECK(m(0,3)==4);
    BOOST_CHECK(m(1,0)==5);BOOST_CHECK(m(1,1)==6);BOOST_CHECK(m(1,2)==7);BOOST_CHECK(m(1,3)==8);
    BOOST_CHECK(m(2,0)==9);BOOST_CHECK(m(2,1)==10);BOOST_CHECK(m(2,2)==11);BOOST_CHECK(m(2,3)==12);

    parser(DataOrder::BottomRightColMajor,s,m);
    BOOST_CHECK(m(0,0)==10);BOOST_CHECK(m(0,1)==7);BOOST_CHECK(m(0,2)==4);BOOST_CHECK(m(0,3)==1);
    BOOST_CHECK(m(1,0)==11);BOOST_CHECK(m(1,1)==8);BOOST_CHECK(m(1,2)==5);BOOST_CHECK(m(1,3)==2);
    BOOST_CHECK(m(2,0)==12);BOOST_CHECK(m(2,1)==9);BOOST_CHECK(m(2,2)==6);BOOST_CHECK(m(2,3)==3);

    parser(DataOrder::BottomRightRowMajor,s,m);
    BOOST_CHECK(m(0,0)==4);BOOST_CHECK(m(0,1)==3);BOOST_CHECK(m(0,2)==2);BOOST_CHECK(m(0,3)==1);
    BOOST_CHECK(m(1,0)==8);BOOST_CHECK(m(1,1)==7);BOOST_CHECK(m(1,2)==6);BOOST_CHECK(m(1,3)==5);
    BOOST_CHECK(m(2,0)==12);BOOST_CHECK(m(2,1)==11);BOOST_CHECK(m(2,2)==10);BOOST_CHECK(m(2,3)==9);
}
