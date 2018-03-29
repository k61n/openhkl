
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
    Eigen::Vector3d kf, ki, q;

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

        kf = state.kfLab(p).rowVector();
        ki = state.ki().rowVector();

        Jk = kf * ki.transpose() / ki.squaredNorm() - Eigen::Matrix3d::Identity();
        double r = dp.norm();

        Jp = Rd*(-1/r * Eigen::Matrix3d::Identity() + 1/r/r/r * dp * dp.transpose());
        Jp *= ki.norm();

        DetectorEvent event(center);
        Jd = state.jacobianQ(event);

        q = kf-ki;
    }
};


ShapeLibrary::ShapeLibrary(): _profiles(), _sigmaD(1e-3), _sigmaE(1e-3), _sigmaM(1e-3), _sigmaA(1e-3)
{   
}

ShapeLibrary::~ShapeLibrary()
{

}

bool ShapeLibrary::addPeak(sptrPeak3D peak, FitProfile&& profile, IntegratedProfile&& integrated_profile)
{
    Eigen::Matrix3d A = peak->getShape().inverseMetric();
    Eigen::Matrix3d cov = 0.5 * (A + A.transpose());
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    Eigen::VectorXd w = solver.eigenvalues();

    // peak has one axis too small. Negated expression to handle nans
    if (!(w.minCoeff() > 1e-2)) {
        return false;
    }
    _profiles[peak].first = std::move(profile);
    _profiles[peak].second = std::move(integrated_profile);
    return true;
}

void ShapeLibrary::updateFit(int num_iterations)
{
    std::vector<std::pair<Eigen::Matrix3d, FitData>> fit_data;
    std::vector<std::pair<int, int>> fit_constraints;
    fit_data.reserve(_profiles.size());

    for (const auto& pair: _profiles) {
        auto peak = pair.first;
        Eigen::Matrix3d cov = peak->getShape().inverseMetric();
        FitData data(peak);
        fit_data.push_back({0.5*(cov+cov.transpose()), data});
    }

    nsx::FitParameters params;

    params.addParameter(&_sigmaD);
    params.addParameter(&_sigmaE);
    params.addParameter(&_sigmaM);
    params.addParameter(&_sigmaA);

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
    static constexpr double deg2 = (M_PI/180)*(M_PI/180);
    Eigen::Matrix3d E;
    E.setIdentity();

    Eigen::Matrix3d cov;
    cov.setZero();

    Eigen::Matrix3d JR = f.Jd.inverse() * f.Rs;

    // beam divergence
    cov += _sigmaD * _sigmaD * (f.kf.squaredNorm()*E - f.kf * f.kf.transpose());
    // energy/lambda
    cov += _sigmaE * _sigmaE * f.kf * f.kf.transpose();
    // mosaicity
    cov += _sigmaM * _sigmaM * (f.q.squaredNorm()*E - f.q * f.q.transpose());
    // ?????
    cov += _sigmaA * _sigmaA * f.q * f.q.transpose();

    cov *= deg2;
    return JR * cov * JR.transpose();
}

double ShapeLibrary::meanPearson() const
{
    double sum_pearson = 0;

    for (const auto& pair: _profiles) {
        auto peak = pair.first;
        Eigen::Matrix3d obs_cov = peak->getShape().inverseMetric();
        Eigen::Matrix3d pred_cov = predictCovariance(peak);
        sum_pearson += (obs_cov.transpose()*pred_cov).trace() / obs_cov.norm() / pred_cov.norm();
    }
    return sum_pearson / _profiles.size();
}

FitProfile ShapeLibrary::meanProfile(const DetectorEvent& ev, double radius, double nframes) const
{
    FitProfile mean;
    PeakList neighbors = findNeighbors(ev, radius, nframes);

    for (auto peak: neighbors) {
        //double weight = (1-r/radius) * (1-df/nframes);
        // mean.addProfile(profile, weight*weight);
        mean.addProfile(_profiles.find(peak)->second.first, 1.0);
    }
    mean.normalize();
    return mean;
}

IntegratedProfile ShapeLibrary::meanIntegratedProfile(const DetectorEvent& ev, double radius, double nframes) const
{
    IntegratedProfile mean;
    PeakList neighbors = findNeighbors(ev, radius, nframes);

    for (auto peak: neighbors) {
        //double weight = (1-r/radius) * (1-df/nframes);
        // mean.addProfile(profile, weight*weight);
        mean.add(_profiles.find(peak)->second.second);
    }
    mean.rescale(1.0 / neighbors.size());
    return mean;
}

PeakList ShapeLibrary::findNeighbors(const DetectorEvent& ev, double radius, double nframes) const
{
    PeakList neighbors;
    Eigen::Vector3d center(ev._px, ev._py, ev._frame);
    
    for (const auto& pair: _profiles) {
        auto peak = pair.first;    
        Eigen::Vector3d dc = center - peak->getShape().center();    
        // too far away on detector
        if (dc(0)*dc(0) + dc(1)*dc(1) > radius*radius) {
            continue;
        }
        // too far away in frame number
        if (std::fabs(dc(2)) > nframes) {
            continue;
        }
        neighbors.push_back(peak);
    }
    if (neighbors.size() == 0) {
        throw std::runtime_error("Error, no neighboring profiles found.");
    }
    return neighbors;
}

Eigen::Matrix3d ShapeLibrary::meanCovariance(sptrPeak3D reference_peak, double radius, double nframes) const
{
    Eigen::Matrix3d cov;
    cov.setZero();
    PeakList neighbors = findNeighbors(DetectorEvent(reference_peak->getShape().center()), radius, nframes);
    PeakCoordinateSystem reference_coord(reference_peak);

    for (auto peak: neighbors) {
        PeakCoordinateSystem coord(peak);
        Eigen::Matrix3d J = coord.jacobian();
        cov += J * peak->getShape().inverseMetric() * J.transpose();
    }

    if (neighbors.size() == 0) {
        throw std::runtime_error("Error, no neighboring profiles found.");
    }

    cov /= neighbors.size();
    Eigen::Matrix3d JI = reference_coord.jacobian().inverse();
    return JI * cov * JI.transpose();
}

} // end namespace nsx
