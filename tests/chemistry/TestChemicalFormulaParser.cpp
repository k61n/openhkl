#define BOOST_TEST_MODULE "Test Formula Parser"
#define BOOST_TEST_DYN_LINK

#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include <nsxlib/chemistry/ChemicalFormulaParser.h>
#include <nsxlib/chemistry/Element.h>

using namespace SX::Chemistry;

BOOST_AUTO_TEST_CASE(Test_Formula_Parser)
{
	for (std::string material : {"C2H{H[1](0.8),H[2](0.2)}3N",
	                             "C",
	                             "CH%4",
	                             "CH4(0.4);H2O(0.6)",
	                             "H[2]2O4",
								 "CH4(0.4);H2O(0.6)"}) {
		try {
			isotopeMixture f;
			ChemicalFormulaParser<std::string::iterator> fp;
			std::cout<<material<<std::endl;
			bool ok = qi::phrase_parse(material.begin(),material.end(),fp,qi::blank,f);
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
