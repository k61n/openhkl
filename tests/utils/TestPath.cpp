#define BOOST_TEST_MODULE "Test Path"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>

#include "Path.h"

using namespace SX::Utils;

BOOST_AUTO_TEST_CASE(Test_Path)
{

	namespace filesystem=boost::filesystem;

	Path* path=Path::Instance();

    std::cout << "hello" << path->getApplicationDataPath() << std::endl;

//	p.clear();
//	p=path->getResourcesPath();
//	std::cout<<p<<std::endl;
//
//	p.clear();
//	p=path->expandUser("~/toto");
//	std::cout<<p<<std::endl;
//
//	p.clear();
//	p=path->getHomeDirectory();
//	std::cout<<p<<std::endl;

}
