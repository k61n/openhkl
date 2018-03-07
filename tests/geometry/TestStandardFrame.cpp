#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/NSXTest.h>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/StandardFrame.h>


int main()
{
    nsx::DataReaderFactory factory;

    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", diff));

    expt->addData(dataf);

    // different places to check the coordinate calculation
    std::vector<Eigen::Vector3d> coords = {
        {200.0, 200.0, 5.0},
        {200.0, 700.0, 5.0},
        {1000.0, 200.0, 5.0},
        {1000.0, 700.0, 5.0},
        {500.0, 200.0, 5.0},
        {500.0, 700.0, 5.0},
        {200.0, 200.0, 10.0},
        {200.0, 700.0, 10.0},
        {1000.0, 200.0, 10.0},
        {1000.0, 700.0, 10.0},
        {500.0, 200.0, 10.0},
        {500.0, 700.0, 10.0},
        {200.0, 200.0, 15.0},
        {200.0, 700.0, 15.0},
        {1000.0, 200.0, 15.0},
        {1000.0, 700.0, 15.0},
        {500.0, 200.0, 15.0},
        {500.0, 700.0, 15.0},
    };
    
    nsx::sptrPeak3D peak(new nsx::Peak3D(dataf));

    for (auto coord: coords) {
        peak->setShape(nsx::Ellipsoid(coord, 2.0));
        nsx::StandardFrame frame(peak);

        auto J = frame.jacobian();

        const double dt = 0.01;

        Eigen::Vector3d e0 = frame.transform(nsx::DetectorEvent(coord[0], coord[1], coord[2]));
        Eigen::Vector3d e1 = frame.transform(nsx::DetectorEvent(coord[0]+dt, coord[1], coord[2]));
        Eigen::Vector3d e2 = frame.transform(nsx::DetectorEvent(coord[0], coord[1]+dt, coord[2]));
        Eigen::Vector3d e3 = frame.transform(nsx::DetectorEvent(coord[0], coord[1], coord[2]+dt));

        Eigen::Vector3d y1 = J*Eigen::Vector3d(dt, 0, 0);
        Eigen::Vector3d y2 = J*Eigen::Vector3d(0, dt, 0);
        Eigen::Vector3d y3 = J*Eigen::Vector3d(0, 0, dt);

        auto detector = peak->data()->diffractometer()->getDetector();

        NSX_CHECK_SMALL(e0.norm(), 1e-12);
        NSX_CHECK_SMALL((e1-y1).norm() / e1.norm(), 1e-5);
        NSX_CHECK_SMALL((e2-y2).norm() / e2.norm(), 1e-5);
        NSX_CHECK_SMALL((e3-y3).norm() / e3.norm(), 1e-5);
        
        auto sigmaD = frame.estimateDivergence();
        auto sigmaM = frame.estimateMosaicity();

        auto detector_shape = frame.detectorShape(sigmaD, sigmaM);

        peak->setShape(detector_shape);

        NSX_CHECK_CLOSE(frame.estimateDivergence(), sigmaD, 1e-7);
        NSX_CHECK_CLOSE(frame.estimateMosaicity(), sigmaM, 1e-7);

        auto standard_shape = frame.standardShape();
        Eigen::Matrix3d cov = standard_shape.inverseMetric();

        NSX_CHECK_CLOSE(cov(0,0), sigmaD*sigmaD, 1e-5);
        NSX_CHECK_CLOSE(cov(1,1), sigmaD*sigmaD, 1e-5);
        NSX_CHECK_CLOSE(cov(2,2), sigmaM*sigmaM, 1e-5);

        NSX_CHECK_SMALL(cov(0,1), 1e-10);
        NSX_CHECK_SMALL(cov(0,2), 1e-10);
        NSX_CHECK_SMALL(cov(1,2), 1e-10);
    }

    

    return 0;
}
