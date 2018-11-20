#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/AutoIndexer.h>
#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/DirectVector.h>
#include <nsxlib/Ellipsoid.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Sample.h>
#include <nsxlib/ShapeLibrary.h>
#include <nsxlib/PixelSumIntegrator.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataset(new nsx::DataSet("hdf", "gal3.hdf", experiment.diffractometer()));

    experiment.addData(dataset);

    Eigen::Matrix3d A;

    A(0,0) = 2.6701917977711296;
    A(0,1) = -14.756687297370734;
    A(0,2) = -52.793228654324409;

    A(1,0) = -58.414811499919281;
    A(1,1) = -0.59943589892690052;
    A(1,2) = -2.7869684459636987;

    A(2,0) = 0.19723512073382329;
    A(2,1) = 64.314912973653236;
    A(2,2) = -17.967236099898955;


    const Eigen::Matrix3d B = A.inverse().transpose();
    const Eigen::Vector3d q0 = Eigen::RowVector3d(-6, 12, -38)*B;
    nsx::Ellipsoid shape(Eigen::Vector3d(434, 802, 10), 2);
    auto peak = std::make_shared<nsx::Peak3D>(dataset, shape);
    peak->setSelected(true);

    Eigen::Vector3d q1 = peak->q().rowVector();
    NSX_CHECK_SMALL((q1-q0).norm() / q0.norm(), 1e-1);

    std::cout << q0.transpose() << std::endl;
    std::cout << q1.transpose() << std::endl;
    
    nsx::PeakList peaks;
    peaks.push_back(peak);
    nsx::PixelSumIntegrator integrator(false, false);
    integrator.integrate(peaks, dataset, 2.7, 3.0, 4.0);

    NSX_CHECK_ASSERT(peak->enabled() == true);

    return 0;
}
