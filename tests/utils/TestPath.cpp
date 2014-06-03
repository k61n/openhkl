#define BOOST_TEST_MODULE "Test Path"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>

#include "Path.h"

using namespace SX::Utils;

BOOST_AUTO_TEST_CASE(Test_Path)
{

	std::string p=getInstallationPath();
	std::cout<<p<<std::endl;

	p=getResourcesPath();
	std::cout<<p<<std::endl;

}
