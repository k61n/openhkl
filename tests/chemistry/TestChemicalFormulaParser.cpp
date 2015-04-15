#define BOOST_TEST_MODULE "Test Formula Parser"
#define BOOST_TEST_DYN_LINK

#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

#include "ChemicalFormulaParser.h"

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_Formula_Parser)
{

	formula f;

	std::string form="C[12]H1.2";

	ChemicalFormulaParser<std::string::iterator> fp;
	qi::phrase_parse(form.begin(),form.end(),fp,qi::blank,f);

}
