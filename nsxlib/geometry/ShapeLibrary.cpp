
#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Ellipsoid.h"
#include "Peak3D.h"
#include "ShapeLibrary.h"



namespace nsx {

struct FitData {
    Eigen::Matrix3d Rs, Rd, A, Jp, Jd;

    FitData(sptrPeak3D peak)
    {
        auto detector = peak->data()->diffractometer()->getDetector();
        Eigen::Vector3d center = peak->getShape().center();
        auto state = peak->data()->interpolatedState(center[2]);

        Rs = state.sampleOrientationMatrix().transpose();
        Rd = state.detectorOrientation;

        auto p = detector->pixelPosition(center[0], center[1]);
        Eigen::Vector3d p0 = state.samplePosition;
        Eigen::Vector3d dp = p.vector()-p0;

        Eigen::Vector3d kf = state.kfLab(p).rowVector();
        Eigen::Vector3d ki = state.ki().rowVector();

        A = kf * ki.transpose() / ki.squaredNorm() - Eigen::Matrix3d::Identity();
        double r = dp.norm();

        Jp = Rd*(-1/r * Eigen::Matrix3d::Identity() + 1/r/r/r * dp * dp.transpose());
        Jp *= ki.norm();
    }
};


ShapeLibrary::ShapeLibrary(): _strongPeaks()
{
    _covBeam.setZero();
    _covMosaicity.setZero();
    _covDetector.setZero();
    _covShape.setZero();
    _covBase.setZero();
}

ShapeLibrary::~ShapeLibrary()
{

}

void ShapeLibrary::addPeak(sptrPeak3D peak)
{
    _strongPeaks.push_back(peak);
}

void ShapeLibrary::updateFit(int num_iterations, double epsilon)
{

}

Eigen::Matrix3d ShapeLibrary::predictCovariance(sptrPeak3D peak)
{
    FitData f(peak);

    Eigen::Matrix3d RA = f.Rs*f.A;
    Eigen::Matrix3d RJ = f.Rs*f.Jp;

    return f.Rs*_covBase*f.Rs.transpose() + _covMosaicity + RA * _covBeam * RA.transpose() + RJ * _covShape * RJ.transpose();
}

} // end namespace nsx
