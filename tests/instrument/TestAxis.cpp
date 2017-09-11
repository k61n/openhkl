#define BOOST_TEST_MODULE "Test Axis"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <nsxlib/instrument/Axis.h>
#include <nsxlib/utils/Units.h>

class TestAxis: public nsx::Axis
{
public:
    TestAxis(const std::string& label, bool physical);
    ~TestAxis();
    TestAxis* clone() const;
    Eigen::Transform<double,3,Eigen::Affine> getHomMatrix(double value) const;
};

TestAxis::TestAxis(const std::string& label, bool physical) : Axis(label)
{
    setPhysical(physical);
}

TestAxis::~TestAxis()
{
}

TestAxis* TestAxis::clone() const
{
    return new TestAxis(*this);
}

Eigen::Transform<double,3,Eigen::Affine> TestAxis::getHomMatrix(double value) const
{
    return Eigen::Transform<double,3,Eigen::Affine>();
}

BOOST_AUTO_TEST_CASE(Tests_Axis)
{
    TestAxis axis("Omega",true);
    const std::string& label=axis.getLabel();

    BOOST_CHECK_EQUAL(label,"Omega");
    Eigen::Vector3d v;
    v << 0,0,1;
    axis.setAxis(v);
    BOOST_CHECK_EQUAL(v,axis.getAxis());

    BOOST_CHECK_EQUAL(axis.getOffset(),0.0);

    axis.setOffset(2.0);
    BOOST_CHECK_EQUAL(axis.getOffset(),2.0);
}
