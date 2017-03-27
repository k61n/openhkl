#define BOOST_TEST_MODULE "Test Formula Parser"
#define BOOST_TEST_DYN_LINK

#define BOOST_SPIRIT_USE_PHOENIX_V3
//#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include <nsxlib/chemistry/ChemicalFormulaParser.h>

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_ChemicalFormula_Parser)
{
	for (const std::string& material : {"C2H{H[1](0.8),H[2](0.2)}3N",
	                             "C",
	                             "CH%4",
	                             "CH4(0.4);H2O(0.6)",
	                             "H[2]2O4",
								 "CH4(0.4);H2O(0.6)"}) {
		try {
			isotopeContents f;
			ChemicalFormulaParser<std::string::const_iterator> fp;
			std::cout<<material<<std::endl;
			bool ok = qi::phrase_parse(material.begin(),material.end(),fp,qi::blank,f);
			std::cout<<ok<<std::endl;
			if (!ok) {
				throw std::runtime_error("");
			}
			for (auto p : f) {
				std::cout << p.first << " --> " << p.second << std::endl;
			}
		} catch (const std::runtime_error&) {
			std::cout<<"Failed to parse "<<material<<std::endl;
		}
		std::cout<<std::endl;
		std::cout<<std::endl;

	}
}
