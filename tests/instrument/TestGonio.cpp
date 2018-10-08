#include <cmath>

#include <Eigen/Dense>

#include <nsxlib/Axis.h>
#include <nsxlib/DirectVector.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>

const double tolerance=1e-6;

NSX_INIT_TEST

int main()
{
    // A simple translation table
    nsx::Gonio t("Translation table");
    t.addTranslation("x",Eigen::Vector3d(1,0,0));
    t.addTranslation("y",Eigen::Vector3d(0,1,0));
    t.addTranslation("z",Eigen::Vector3d(0,0,1));

    t.axis("x")->setPhysical(true);
    t.axis("y")->setPhysical(true);
    t.axis("z")->setPhysical(true);

    // Transform (0,0,0)
    nsx::DirectVector result = t.transform(nsx::DirectVector(0.0,0.0,0.0),{1,2,3});
    NSX_CHECK_CLOSE(result[0],1,tolerance);
    NSX_CHECK_CLOSE(result[1],2,tolerance);
    NSX_CHECK_CLOSE(result[2],3,tolerance);
    // Check that throws if the number of parameters is invalid
    NSX_CHECK_THROW(t.transform(nsx::DirectVector(0.0,0.0,0.0),{1,2}),std::range_error);

    // Simple goniometer as in Busing Levy
    nsx::Gonio g("Busing Levy convention");
    g.addRotation("omega",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    g.addRotation("chi",Eigen::Vector3d(0,1,0),nsx::RotAxis::CCW);
    g.addRotation("phi",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);

    // Check that result of combined rotation is Ok.
    double om=12*nsx::deg;
    double chi=24*nsx::deg;
    double phi=55*nsx::deg;
    Eigen::Matrix3d OM;
    OM << cos(om), sin(om), 0,
         -sin(om), cos(om), 0,
          0      ,  0      , 1;
    Eigen::Matrix3d CH;
    CH << cos(chi), 0, sin(chi),
          0 ,       1,      0,
         -sin(chi), 0, cos(chi);
    Eigen::Matrix3d PH;
    PH << cos(phi), sin(phi), 0,
         -sin(phi), cos(phi), 0,
          0      ,  0      , 1;

    Eigen::Vector3d result1 = OM*CH*PH*Eigen::Vector3d(1,0,0);

    result = g.transform(nsx::DirectVector(1.0,0.0,0.0),{om,chi,phi});
    NSX_CHECK_CLOSE(result[0],result1[0],tolerance);
    NSX_CHECK_CLOSE(result[1],result1[1],tolerance);
    NSX_CHECK_CLOSE(result[2],result1[2],tolerance);

    // Check that this works with row vector as well,
    nsx::DirectVector result2 = g.transform(nsx::DirectVector(1.0,0.0,0.0),{om,chi,phi});
    NSX_CHECK_CLOSE(result[0],result2[0],tolerance);
    NSX_CHECK_CLOSE(result[1],result2[1],tolerance);
    NSX_CHECK_CLOSE(result[2],result2[2],tolerance);

    return 0;
}
