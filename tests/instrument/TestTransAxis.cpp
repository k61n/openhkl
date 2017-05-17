#define BOOST_TEST_MODULE "Test Translation Axis"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/instrument/TransAxis.h>
#include <nsxlib/utils/Units.h>

using Eigen::Vector3d;

using namespace nsx;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Trans_Axis)
{
    TransAxis t("x",Vector3d(1,0,0));

    // Translate the vector 0,1,2 by 0.4
    Vector3d v=t.transform(Vector3d(0,1,2),0.4);

    BOOST_CHECK_CLOSE(v[0],0.4,tolerance);
    BOOST_CHECK_CLOSE(v[1],1.0,tolerance);
    BOOST_CHECK_CLOSE(v[2],2.0,tolerance);
}
