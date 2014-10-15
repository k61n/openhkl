#define BOOST_TEST_MODULE "Test Component State"
#define BOOST_TEST_DYN_LINK
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include "Sample.h"
#include "ComponentState.h"
#include "Gonio.h"
#include <iostream>

using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;

// 0.1% error
const double tolerance=1e-3;
BOOST_AUTO_TEST_CASE(Test_Component_State)
{
	std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
	g->addRotation("gamma",Vector3d(0,0,1),RotAxis::CW);

	Sample sample;
	sample.setGonio(g);

	ComponentState cs = sample.createState({2.0});

	ComponentState* pcs(new ComponentState(cs));

}

