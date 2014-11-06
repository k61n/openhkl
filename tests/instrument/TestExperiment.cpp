#define BOOST_TEST_MODULE "Test Experiment"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "Experiment.h"
#include "Diffractometer.h"
#include "ILLAsciiData.h"

using namespace SX::Instrument;

BOOST_AUTO_TEST_CASE(Test_Experiment)
{

	Experiment exp("my-exp","D10 4-circles");

	BOOST_CHECK_EQUAL(exp.getName(),"my-exp");

	// Change the name of the experiment
	exp.setName("toto");
	BOOST_CHECK_EQUAL(exp.getName(),"toto");

	// The data must be empty at experiment creation
	BOOST_CHECK_EQUAL(exp.getDataNames().size(),0);

	// Add some data
	ILLAsciiData* data = new ILLAsciiData(std::string("D10_ascii_example"),exp.getDiffractometer(),false);
	exp.addData(data);
	BOOST_CHECK_EQUAL(exp.getDataNames().size(),1);
	BOOST_CHECK_EQUAL(exp.getDataNames()[0],"D10_ascii_example");

	// Check that adding the same data is now taken into account
	exp.addData(data);
	exp.addData(data);
	exp.addData(data);
	BOOST_CHECK_EQUAL(exp.getDataNames().size(),1);

	BOOST_CHECK_EQUAL(exp.getData("D10_ascii_example")->getBasename(),"D10_ascii_example");

	// Remove the data from the experiment
	exp.removeData("D10_ascii_example");
	// The data must be empty again after data deletion
	BOOST_CHECK_EQUAL(exp.getDataNames().size(),0);

}

