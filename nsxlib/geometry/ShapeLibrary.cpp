
#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Ellipsoid.h"
#include "Minimizer.h"
#include "Peak3D.h"
#include "ShapeLibrary.h"


#include <Eigen/Core>

namespace nsx {

struct FitData {
    Eigen::Matrix3d Rs, Rd, Jk, Jp, Jd;
    Eigen::Vector3d q;

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

        Jk = kf * ki.transpose() / ki.squaredNorm() - Eigen::Matrix3d::Identity();
        double r = dp.norm();

        Jp = Rd*(-1/r * Eigen::Matrix3d::Identity() + 1/r/r/r * dp * dp.transpose());
        Jp *= ki.norm();

        DetectorEvent event(center);
        Jd = state.jacobianQ(event);

        q = kf-ki;
    }
};


ShapeLibrary::ShapeLibrary(): _strongPeaks()
{
    _covDetector.setZero();
    _covMosaicity.setZero();
    _covScatter.setZero();    
}

ShapeLibrary::~ShapeLibrary()
{

}

bool ShapeLibrary::addPeak(sptrPeak3D peak)
{
    Eigen::Matrix3d A = peak->getShape().inverseMetric();
    Eigen::Matrix3d cov = 0.5 * (A + A.transpose());

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    Eigen::VectorXd w = solver.eigenvalues();

    // peak has one axis too small. Negated expression to handle nans
    if (!(w.minCoeff() > 1e-2)) {
        return false;
    }
    
    _strongPeaks.push_back(peak);
    return true;
}

void ShapeLibrary::updateFit(int num_iterations)
{
    std::vector<std::pair<Eigen::Matrix3d, FitData>> fit_data;
    std::vector<std::pair<int, int>> fit_constraints;
    fit_data.reserve(_strongPeaks.size());

    for (auto peak: _strongPeaks) {
        auto cov = peak->getShape().inverseMetric();
        FitData data(peak);
        fit_data.push_back({0.5*(cov+cov.transpose()), data});
    }

    nsx::FitParameters params;

    for (auto i = 0; i < 3; ++i) {
        for (auto j = i; j < 3; ++j) {
            params.addParameter(&_covDetector(i,j));
            params.addParameter(&_covMosaicity(i,j));
        }
    }

    for (auto i = 0; i < 6; ++i) {
        for (auto j = i; j < 6; ++j) {
            params.addParameter(&_covScatter(i,j));
        }
    }

    auto residual = [&](Eigen::VectorXd& r) -> int {
        int k = 0;

        for (const auto& tup: fit_data) {
            const auto& cov = tup.first;
            const auto& data = tup.second;

            Eigen::Matrix3d pred_cov = predictCovariance(data);

            Eigen::Matrix3d delta = cov - pred_cov;

            for (int i = 0; i < 3; ++i) {
                for (int j = i; j < 3; ++j) {
                    r(k++) = delta(i,j);
                }
            }
        }

        return 0;
    };

    nsx::Minimizer min;
    min.initialize(params, 6*fit_data.size());
    min.set_f(residual);
    min.fit(num_iterations);
}

Eigen::Matrix3d ShapeLibrary::predictCovariance(sptrPeak3D peak) const
{
    FitData f(peak);
    return predictCovariance(f);
}

Eigen::Matrix3d ShapeLibrary::predictCovariance(const FitData& f) const
{
    Eigen::Matrix3d CD = 0.5 * (_covDetector + _covDetector.transpose());
    Eigen::Matrix3d CM = 0.5 * (_covMosaicity + _covMosaicity.transpose());
    Eigen::Matrix<double, 6, 6> CS = 0.5 * (_covScatter + _covScatter.transpose());

    const auto& q = f.q;
    const double q2 = q.squaredNorm();
    const auto& R = f.Rs;
    const Eigen::Matrix3d P = q2*Eigen::Matrix3d::Identity() - q*q.transpose();
    const Eigen::Matrix3d RPR = R*P*R;

    Eigen::Matrix<double, 3, 6> delta;
    delta.block(0,0,3,3) = R * f.Jk;
    delta.block(0,3,3,3) = R * f.Jp;

    const Eigen::Matrix3d mos = q2 * RPR * CM * RPR;
    const Eigen::Matrix3d scatter = delta * CS * delta.transpose();

    const Eigen::Matrix3d JDI = f.Jd.inverse();

    Eigen::Matrix3d detector_cov = JDI * (mos + scatter) * JDI.transpose();
    detector_cov += CD;

    return CD + JDI*(mos+scatter)*JDI.transpose();
}

double ShapeLibrary::meanPearson() const
{
    double sum_pearson = 0;

    for (auto peak: _strongPeaks) {
        Eigen::Matrix3d obs_cov = peak->getShape().inverseMetric();
        Eigen::Matrix3d pred_cov = predictCovariance(peak);
        sum_pearson += (obs_cov.transpose()*pred_cov).trace() / obs_cov.norm() / pred_cov.norm();
    }
    return sum_pearson / _strongPeaks.size();
}

} // end namespace nsx
