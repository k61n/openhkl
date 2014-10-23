#define BOOST_TEST_MODULE "Test Experiment"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "Experiment.h"
#include "Diffractometer.h"
#include "DiffractometerFactory.h"
#include "ILLAsciiData.h"

using namespace SX::Instrument;

BOOST_AUTO_TEST_CASE(Test_Experiment)
{

	DiffractometerFactory* factory = DiffractometerFactory::Instance();

	Diffractometer* diff = factory->create("D19-4c","my d19 diff");

	std::shared_ptr<Diffractometer> sptr(new Diffractometer(*diff));

	Experiment exp("my-exp",sptr);

	BOOST_CHECK_EQUAL(exp.getName(),"my-exp");
	BOOST_CHECK_EQUAL(exp.getDiffractometer(),sptr);

	ILLAsciiData* data = new ILLAsciiData(std::string("D10_ascii_example"),sptr,false);

	BOOST_CHECK_EQUAL(exp.getDataNames().size(),0);

	exp.addData(data);

	BOOST_CHECK_EQUAL(exp.getDataNames().size(),1);
	BOOST_CHECK_EQUAL(exp.getDataNames()[0],"D10_ascii_example");

	// Check that adding the same data is now taken into account
	exp.addData(data);
	exp.addData(data);
	exp.addData(data);
	BOOST_CHECK_EQUAL(exp.getDataNames().size(),1);

	BOOST_CHECK_EQUAL(exp.getData("D10_ascii_example")->getBasename(),"D10_ascii_example");

}

