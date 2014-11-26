#define BOOST_TEST_MODULE "Test Eigen Matrix Parser"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

#include "EigenMatrixParser.h"
#include <Eigen/Dense>

using namespace SX::Utils;
using namespace Eigen;

BOOST_AUTO_TEST_CASE(Test_EigenMatrixParser)
{

	Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::ColMajor> m;
	m.resize(3,4);
	std::string s("1 2 3 4 5 6 7 8 9 10 11 12");

	EigenMatrixParser<std::string::iterator,BottomLeftRowMajorMapper> parser1;
	qi::phrase_parse(s.begin(),s.end(),parser1,qi::blank, m);
    BOOST_CHECK(m(0,0)==1);BOOST_CHECK(m(0,1)==2);BOOST_CHECK(m(0,2)==3);BOOST_CHECK(m(0,3)==4);
    BOOST_CHECK(m(1,0)==5);BOOST_CHECK(m(1,1)==6);BOOST_CHECK(m(1,2)==7);BOOST_CHECK(m(1,3)==8);
    BOOST_CHECK(m(2,0)==9);BOOST_CHECK(m(2,1)==10);BOOST_CHECK(m(2,2)==11);BOOST_CHECK(m(2,3)==12);

    EigenMatrixParser<std::string::iterator,BottomLeftColMajorMapper> parser2;
	qi::phrase_parse(s.begin(),s.end(),parser2,qi::blank, m);
	BOOST_CHECK(m(0,0)==1);BOOST_CHECK(m(0,1)==4);BOOST_CHECK(m(0,2)==7);BOOST_CHECK(m(0,3)==10);
	BOOST_CHECK(m(1,0)==2);BOOST_CHECK(m(1,1)==5);BOOST_CHECK(m(1,2)==8);BOOST_CHECK(m(1,3)==11);
	BOOST_CHECK(m(2,0)==3);BOOST_CHECK(m(2,1)==6);BOOST_CHECK(m(2,2)==9);BOOST_CHECK(m(2,3)==12);


    EigenMatrixParser<std::string::iterator,TopLeftRowMajorMapper> parser3;
	qi::phrase_parse(s.begin(),s.end(),parser3,qi::blank, m);
	BOOST_CHECK(m(0,0)==9);BOOST_CHECK(m(0,1)==10);BOOST_CHECK(m(0,2)==11);BOOST_CHECK(m(0,3)==12);
	BOOST_CHECK(m(1,0)==5);BOOST_CHECK(m(1,1)==6);BOOST_CHECK(m(1,2)==7);BOOST_CHECK(m(1,3)==8);
	BOOST_CHECK(m(2,0)==1);BOOST_CHECK(m(2,1)==2);BOOST_CHECK(m(2,2)==3);BOOST_CHECK(m(2,3)==4);

	EigenMatrixParser<std::string::iterator,TopLeftColMajorMapper> parser4;
	qi::phrase_parse(s.begin(),s.end(),parser4,qi::blank, m);
	BOOST_CHECK(m(0,0)==3);BOOST_CHECK(m(0,1)==6);BOOST_CHECK(m(0,2)==9);BOOST_CHECK(m(0,3)==12);
	BOOST_CHECK(m(1,0)==2);BOOST_CHECK(m(1,1)==5);BOOST_CHECK(m(1,2)==8);BOOST_CHECK(m(1,3)==11);
	BOOST_CHECK(m(2,0)==1);BOOST_CHECK(m(2,1)==4);BOOST_CHECK(m(2,2)==7);BOOST_CHECK(m(2,3)==10);

	EigenMatrixParser<std::string::iterator,TopRightRowMajorMapper> parser5;
	qi::phrase_parse(s.begin(),s.end(),parser5,qi::blank, m);
	BOOST_CHECK(m(0,0)==12);BOOST_CHECK(m(0,1)==11);BOOST_CHECK(m(0,2)==10);BOOST_CHECK(m(0,3)==9);
	BOOST_CHECK(m(1,0)==8);BOOST_CHECK(m(1,1)==7);BOOST_CHECK(m(1,2)==6);BOOST_CHECK(m(1,3)==5);
	BOOST_CHECK(m(2,0)==4);BOOST_CHECK(m(2,1)==3);BOOST_CHECK(m(2,2)==2);BOOST_CHECK(m(2,3)==1);

	EigenMatrixParser<std::string::iterator,TopRightColMajorMapper> parser6;
	qi::phrase_parse(s.begin(),s.end(),parser6,qi::blank, m);
	BOOST_CHECK(m(0,0)==12);BOOST_CHECK(m(0,1)==9);BOOST_CHECK(m(0,2)==6);BOOST_CHECK(m(0,3)==3);
	BOOST_CHECK(m(1,0)==11);BOOST_CHECK(m(1,1)==8);BOOST_CHECK(m(1,2)==5);BOOST_CHECK(m(1,3)==2);
	BOOST_CHECK(m(2,0)==10);BOOST_CHECK(m(2,1)==7);BOOST_CHECK(m(2,2)==4);BOOST_CHECK(m(2,3)==1);

	EigenMatrixParser<std::string::iterator,BottomRightRowMajorMapper> parser7;
	qi::phrase_parse(s.begin(),s.end(),parser7,qi::blank, m);
	BOOST_CHECK(m(0,0)==4);BOOST_CHECK(m(0,1)==3);BOOST_CHECK(m(0,2)==2);BOOST_CHECK(m(0,3)==1);
	BOOST_CHECK(m(1,0)==8);BOOST_CHECK(m(1,1)==7);BOOST_CHECK(m(1,2)==6);BOOST_CHECK(m(1,3)==5);
	BOOST_CHECK(m(2,0)==12);BOOST_CHECK(m(2,1)==11);BOOST_CHECK(m(2,2)==10);BOOST_CHECK(m(2,3)==9);

	EigenMatrixParser<std::string::iterator,BottomRightColMajorMapper> parser8;
	qi::phrase_parse(s.begin(),s.end(),parser8,qi::blank, m);
	BOOST_CHECK(m(0,0)==10);BOOST_CHECK(m(0,1)==7);BOOST_CHECK(m(0,2)==4);BOOST_CHECK(m(0,3)==1);
	BOOST_CHECK(m(1,0)==11);BOOST_CHECK(m(1,1)==8);BOOST_CHECK(m(1,2)==5);BOOST_CHECK(m(1,3)==2);
	BOOST_CHECK(m(2,0)==12);BOOST_CHECK(m(2,1)==9);BOOST_CHECK(m(2,2)==6);BOOST_CHECK(m(2,3)==3);

}
