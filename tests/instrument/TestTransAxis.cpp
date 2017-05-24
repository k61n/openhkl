#define BOOST_TEST_MODULE "Test Translation Axis"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/instrument/TransAxis.h>

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Trans_Axis)
{
    nsx::TransAxis t("x",Eigen::Vector3d(1,0,0));

    // Translate the vector 0,1,2 by 0.4
    Eigen::Vector3d v=t.transform(Eigen::Vector3d(0,1,2),0.4);

    BOOST_CHECK_CLOSE(v[0],0.4,tolerance);
    BOOST_CHECK_CLOSE(v[1],1.0,tolerance);
    BOOST_CHECK_CLOSE(v[2],2.0,tolerance);
}
