#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <core/AutoIndexer.h>
#include <core/CrystalTypes.h>
#include <core/DataSet.h>
#include <core/DetectorEvent.h>
#include <core/Diffractometer.h>
#include <core/DirectVector.h>
#include <core/Ellipsoid.h>
#include <core/Experiment.h>
#include <core/NSXTest.h>
#include <core/Peak3D.h>
#include <core/PeakFilter.h>
#include <core/PeakFinder.h>
#include <core/ReciprocalVector.h>
#include <core/Sample.h>
#include <core/ShapeLibrary.h>
#include <core/PixelSumIntegrator.h>
#include <core/UnitCell.h>
#include <core/Units.h>

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
