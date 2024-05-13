//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/ShapeModel.cpp
//! @brief     Implements classes PeakInterpolation, ShapeModel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/ShapeModel.h"

#include "base/fit/Minimizer.h"
#include "base/geometry/DirectVector.h"
#include "base/geometry/Ellipsoid.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Logger.h"
#include "base/utils/ProgressHandler.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Source.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/PeakCollection.h"
#include "tables/crystal/UnitCell.h"

#include <stdexcept>

#include <Eigen/Dense>

namespace ohkl {

void ShapeModelParameters::log(const Level& level) const
{
    IntegrationParameters::log(level);
    ohklLog(level, "Shape Collection parameters:");
    ohklLog(level, "d_min                  = ", d_min);
    ohklLog(level, "d_max                  = ", d_max);
    ohklLog(level, "strength_min           = ", strength_min);
    ohklLog(level, "kabsch_coords          = ", kabsch_coords);
    ohklLog(level, "nbins_x                = ", nbins_x);
    ohklLog(level, "nbins_y                = ", nbins_y);
    ohklLog(level, "nbins_z                = ", nbins_z);
    ohklLog(level, "n_subdiv               = ", n_subdiv);
    ohklLog(level, "sigma_m                = ", sigma_m);
    ohklLog(level, "sigma_d                = ", sigma_d);
    ohklLog(level, "neighbour_range_pixels = ", neighbour_range_pixels);
    ohklLog(level, "neighbour_range_frames = ", neighbour_range_frames);
}

static Eigen::Matrix3d from_cholesky(const std::array<double, 6>& components)
{
    Eigen::Matrix3d L;
    L.setZero();
    L(0, 0) = components[0];
    L(1, 1) = components[1];
    L(2, 2) = components[2];
    L(1, 0) = components[3];
    L(2, 0) = components[4];
    L(2, 1) = components[5];

    return L * L.transpose();
}

//! Helper struct used internally by the shape model.
struct FitData {
    //! Sample orientation matrix
    Eigen::Matrix3d Rs;
    //! Detector orientation matrix
    Eigen::Matrix3d Rd;
    //! Jacobian of kf
    Eigen::Matrix3d Jk;
    //! Jacobian of p
    Eigen::Matrix3d Jp;
    //! Jacobian of (x,y,f) -> q
    Eigen::Matrix3d Jd;
    //! kf
    Eigen::Vector3d kf;
    //! ki
    Eigen::Vector3d ki;
    //! Momentum transfer q
    Eigen::Vector3d q;

    //! Construct a FitData instance directly from a peak.
    explicit FitData(Peak3D* peak)
    {
        const auto* detector = peak->dataSet()->diffractometer()->detector();
        const Eigen::Vector3d center = peak->shape().center();
        const auto state =
            InterpolatedState::interpolate(peak->dataSet()->instrumentStates(), center[2]);

        Rs = state.sampleOrientationMatrix().transpose();
        Rd = state.detectorOrientation;

        const auto p = detector->pixelPosition(center[0], center[1]);
        const Eigen::Vector3d p0 = state.samplePosition;
        const Eigen::Vector3d dp = p.vector() - p0;

        kf = state.kfLab(p).rowVector();
        ki = state.ki().rowVector();

        Jk = kf * ki.transpose() / ki.squaredNorm() - Eigen::Matrix3d::Identity();
        const double r = dp.norm();

        Jp = Rd * (-1 / r * Eigen::Matrix3d::Identity() + 1 / r / r / r * dp * dp.transpose());
        Jp *= ki.norm();

        const DetectorEvent event(center);
        Jd = state.jacobianQ(event.px, event.py);

        q = kf - ki;
    }
};

ShapeModel::ShapeModel()
    : _id(0), _profiles(), _choleskyD(), _choleskyM(), _choleskyS(), _handler(nullptr)
{
    _choleskyD.fill(1e-6);
    _choleskyM.fill(1e-6);
    _choleskyS.fill(1e-6);
    _params = std::make_shared<ShapeModelParameters>();
}

ShapeModel::ShapeModel(const std::string& name) : ShapeModel()
{
    _name = name;
}

ShapeModel::ShapeModel(std::shared_ptr<ShapeModelParameters> params)
    : _id(0)
    , _profiles()
    , _choleskyD()
    , _choleskyM()
    , _choleskyS()
    , _params(params)
    , _handler(nullptr)

{
    _choleskyD.fill(1e-6);
    _choleskyM.fill(1e-6);
    _choleskyS.fill(1e-6);
    _params->log(Level::Info);
}

void ShapeModel::setId(unsigned int id)
{
    if (_id == 0)
        _id = id;
}

static void covariance_helper(
    Eigen::Matrix3d& result, const FitData& f, const Eigen::Matrix3d& sigmaD,
    const Eigen::Matrix3d& sigmaM, const Eigen::Matrix3d& sigmaS)
{
    Eigen::Matrix3d E;
    E.setIdentity();

    Eigen::Matrix3d cov;
    cov.setZero();

    Eigen::Matrix3d Jd = f.Jd.inverse();
    Eigen::Matrix3d Jk = f.Rs * f.Jk;
    Eigen::Matrix3d Jp = f.Rs * f.Jp;
    Eigen::Vector3d qs = f.Rs * f.q;

    Eigen::Matrix3d P = f.q.squaredNorm() * Eigen::Matrix3d::Identity() - qs * qs.transpose();

    // mosaicity
    cov += P * sigmaM * P.transpose();
    // beam divergence
    cov += f.ki.squaredNorm() * Jk * sigmaD * Jk.transpose();
    // shape
    cov += Jp * sigmaS * Jp.transpose();

    result = Jd * cov * Jd.transpose();
}

bool ShapeModel::addPeak(Peak3D* peak, Profile3D&& profile, Profile1D&& integrated_profile)
{
    Eigen::Matrix3d A = peak->shape().inverseMetric();
    Eigen::Matrix3d cov = 0.5 * (A + A.transpose());
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    Eigen::VectorXd w = solver.eigenvalues();

    // peak has one axis too small. Negated expression to handle nans
    if (!(w.minCoeff() > 1e-2))
        return false;
    _profiles[peak].first = std::move(profile);
    _profiles[peak].second = std::move(integrated_profile);
    return true;
}

void ShapeModel::updateFit(int num_iterations)
{
    std::vector<std::pair<Eigen::Matrix3d, FitData>> fit_data;
    std::vector<std::pair<int, int>> fit_constraints;
    fit_data.reserve(_profiles.size());

    for (const auto& pair : _profiles) {
        auto peak = pair.first;
        Eigen::Matrix3d cov = peak->shape().inverseMetric();
        FitData data(peak);
        fit_data.push_back({0.5 * (cov + cov.transpose()), data});
    }

    ohkl::FitParameters params;

    for (auto i = 0; i < 6; ++i) {
        params.addParameter(&_choleskyD[i]);
        params.addParameter(&_choleskyM[i]);
        params.addParameter(&_choleskyS[i]);
    }

    auto residual = [&](Eigen::VectorXd& r) -> int {
        int k = 0;
        Eigen::Matrix3d pred_cov;
        Eigen::Matrix3d sigmaM = from_cholesky(_choleskyM);
        Eigen::Matrix3d sigmaD = from_cholesky(_choleskyD);
        Eigen::Matrix3d sigmaS = from_cholesky(_choleskyS);

        for (const auto& tup : fit_data) {
            const auto& cov = tup.first;
            const auto& data = tup.second;

            covariance_helper(pred_cov, data, sigmaD, sigmaM, sigmaS);
            Eigen::Matrix3d delta = cov - pred_cov;

            for (int i = 0; i < 3; ++i) {
                for (int j = i; j < 3; ++j)
                    r(k++) = delta(i, j);
            }
        }

        return 0;
    };

    ohkl::Minimizer min;
    min.initialize(params, 6 * fit_data.size());
    min.set_f(residual);
    min.fit(num_iterations);
}

void ShapeModel::setParameters(std::shared_ptr<ShapeModelParameters> params)
{
    _params.reset();
    _params = params;
    _params->log(Level::Info);
}

Eigen::Matrix3d ShapeModel::predictCovariance(Peak3D* peak) const
{
    FitData f(peak);
    return predictCovariance(f);
}

Eigen::Matrix3d ShapeModel::predictCovariance(const FitData& f) const
{
    Eigen::Matrix3d result;
    Eigen::Matrix3d sigmaM = from_cholesky(_choleskyM);
    Eigen::Matrix3d sigmaD = from_cholesky(_choleskyD);
    Eigen::Matrix3d sigmaS = from_cholesky(_choleskyS);
    covariance_helper(result, f, sigmaD, sigmaM, sigmaS);
    return result;
}

double ShapeModel::meanPearson() const
{
    double sum_pearson = 0;

    for (const auto& pair : _profiles) {
        auto peak = pair.first;
        Eigen::Matrix3d obs_cov = peak->shape().inverseMetric();
        Eigen::Matrix3d pred_cov = predictCovariance(peak);
        sum_pearson += (obs_cov.transpose() * pred_cov).trace() / obs_cov.norm() / pred_cov.norm();
    }
    return sum_pearson / _profiles.size();
}

Profile3D ShapeModel::meanProfile(const DetectorEvent& ev) const
{
    Profile3D mean;
    auto neighbors = findNeighbors(ev);

    if (neighbors.size() == 1)
        return _profiles.find(neighbors[0])->second.first;

    for (auto peak : neighbors) {

        double weight;
        switch (_params->interpolation) {
            case (PeakInterpolation::NoInterpolation): {
                weight = 1.0;
                break;
            }
            case (PeakInterpolation::InverseDistance): {
                auto state = InterpolatedState::interpolate(_data->instrumentStates(), ev.frame);
                const auto* detector = _data->diffractometer()->detector();
                DirectVector det_pos(detector->pixelPosition(ev.px, ev.py));
                ReciprocalVector ev_q = state.sampleQ(det_pos);
                Eigen::RowVector3d dq = ev_q.rowVector() - peak->q().rowVector();
                weight = 1.0 / dq.norm();
                break;
            }
            case (PeakInterpolation::Intensity): {
                auto corrected_intensity = peak->correctedSumIntensity();
                double intensity = corrected_intensity.value();
                double sigma = corrected_intensity.sigma();
                weight = intensity / sigma;
                break;
            }
            default: {
                weight = 1.0;
                break;
            }
        }

        mean.addProfile(_profiles.find(peak)->second.first, weight);
    }

    mean.normalize();
    return mean;
}

std::vector<Intensity> ShapeModel::meanProfile1D(const DetectorEvent& ev) const
{
    auto neighbors = findNeighbors(ev);
    std::vector<Intensity> mean_profile;
    const double inv_N = 1.0 / neighbors.size();

    if (neighbors.size() == 1)
        return _profiles.find(neighbors[0])->second.second.profile();

    for (auto peak : neighbors) {
        const auto& profile = _profiles.find(peak)->second.second.profile();

        if (mean_profile.empty())
            mean_profile.resize(profile.size());

        for (size_t i = 0; i < mean_profile.size(); ++i)
            mean_profile[i] += profile[i] * inv_N;
    }
    return mean_profile;
}

std::vector<Peak3D*> ShapeModel::findNeighbors(const DetectorEvent& ev) const
{
    std::vector<Peak3D*> neighbors;
    Eigen::Vector3d center(ev.px, ev.py, ev.frame);
    Peak3D* nearest;
    double min_dist_sq = _data->nCols() * _data->nCols() + _data->nRows() * _data->nRows();
    double min_frames = _data->nFrames();
    double radius_sq = _params->neighbour_range_pixels * _params->neighbour_range_pixels;

    for (const auto& pair : _profiles) {
        auto peak = pair.first;
        Eigen::Vector3d diff = center - peak->shape().center();
        double pix_dist_sq = diff(0) * diff(0) + diff(1) * diff(1);
        double frame_dist = std::fabs(diff(2));

        // too far away on detector
        if (pix_dist_sq < min_dist_sq && frame_dist < min_frames) {
            min_dist_sq = pix_dist_sq;
            min_frames = frame_dist;
            nearest = peak;
        }

        if (pix_dist_sq < radius_sq && frame_dist < _params->neighbour_range_frames)
            neighbors.push_back(peak);
    }

    if (neighbors.empty())
        neighbors.push_back(nearest);

    return neighbors;
}

Eigen::Matrix3d ShapeModel::meanCovariance(Peak3D* reference_peak) const
{
    Eigen::Matrix3d cov;
    cov.setZero();
    auto neighbors = findNeighbors(DetectorEvent(reference_peak->shape().center()));

    PeakCoordinateSystem reference_coord(reference_peak);

    double sum_weight(0.0);
    for (auto peak : neighbors) {
        PeakCoordinateSystem coord(peak);
        Eigen::Matrix3d J = coord.jacobian();

        double weight;
        switch (_params->interpolation) {
            case (PeakInterpolation::NoInterpolation): {
                weight = 1.0;
                break;
            }
            case (PeakInterpolation::InverseDistance): {
                Eigen::RowVector3d dq = reference_peak->q().rowVector() - peak->q().rowVector();
                weight = 1.0 / dq.norm();
                break;
            }
            case (PeakInterpolation::Intensity): {
                auto corrected_intensity = peak->correctedSumIntensity();
                double intensity = corrected_intensity.value();
                double sigma = corrected_intensity.sigma();
                weight = intensity / sigma;
                break;
            }
            default: {
                reference_peak->setRejectionFlag(RejectionFlag::InterpolationFailure);
                return {};
            }
        }

        cov += weight * (J * peak->shape().inverseMetric() * J.transpose());

        sum_weight += weight;
    }

    cov /= sum_weight;

    Eigen::Matrix3d JI = reference_coord.jacobian().inverse();
    return JI * cov * JI.transpose();
}

void ShapeModel::setPredictedShapes(PeakCollection* peaks)
{
    ohklLog(
        Level::Info, "ShapeModel: Computing shapes of ", peaks->numberOfPeaks(),
        " calculated peaks");

    int count = 0;
    int npeaks = peaks->numberOfPeaks();
    std::ostringstream oss;
    oss << "Computing shapes of " << npeaks << " peaks";
    if (_handler) {
        _handler->setStatus(oss.str().c_str());
        _handler->setProgress(0);
    }

#pragma omp parallel for
    for (auto peak : peaks->getPeakList()) {
        // Skip the peak if any error occur when computing its mean covariance (e.g.
        // too few or no neighbouring peaks found)
        auto cov = meanCovariance(peak);
        Eigen::Vector3d center = peak->shape().center();
        peak->setShape(Ellipsoid(center, cov.inverse()));

        if (_handler) {
            double progress = ++count * 100.0 / npeaks;
            _handler->setProgress(progress);
        }
    }
    ohklLog(Level::Info, "ShapeModel: finished computing shapes");
}

std::array<double, 6> ShapeModel::choleskyD() const
{
    return _choleskyD;
}

std::array<double, 6> ShapeModel::choleskyM() const
{
    return _choleskyM;
}

std::array<double, 6> ShapeModel::choleskyS() const
{
    return _choleskyS;
}

std::map<Peak3D*, std::pair<Profile3D, Profile1D>> ShapeModel::profiles() const
{
    return _profiles;
}

ShapeModelParameters* ShapeModel::parameters()
{
    return _params.get();
}

bool ShapeModel::detectorCoords() const
{
    return !_params->kabsch_coords;
}

AABB ShapeModel::getAABB()
{
    AABB aabb;
    if (_params->kabsch_coords) {
        const Eigen::Vector3d sigma(_params->sigma_d, _params->sigma_d, _params->sigma_m);
        aabb.setLower(-_params->peak_end * sigma);
        aabb.setUpper(_params->peak_end * sigma);
    } else {
        const Eigen::Vector3d dx(_params->nbins_x, _params->nbins_y, _params->nbins_z);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }
    return aabb;
}

void ShapeModel::integrate(
    std::vector<Peak3D*> peaks, const sptrDataSet data, sptrProgressHandler handler)
{
    ohklLog(Level::Info, "ShapeModel::integrate: integrating ", peaks.size(), " peaks");
    _data = data;
    ShapeIntegrator integrator(
        this, getAABB(), _params->nbins_x, _params->nbins_y, _params->nbins_z);
    ohkl::IntegrationParameters int_params{};
    int_params.region_type = _params->region_type;
    int_params.peak_end = _params->peak_end;
    int_params.bkg_begin = _params->bkg_begin;
    int_params.bkg_end = _params->bkg_end;
    int_params.fixed_peak_end = _params->fixed_peak_end;
    int_params.fixed_bkg_begin = _params->fixed_bkg_begin;
    int_params.fixed_bkg_end = _params->fixed_bkg_end;
    integrator.setHandler(handler);
    integrator.setParameters(int_params);

    integrator.integrate(peaks, this, data);
    ohklLog(Level::Info, "ShapeModel::integrate: finished integrating shapes");
}

void ShapeModel::build(PeakCollection* peaks, sptrDataSet data)
{
    ohklLog(Level::Info, "ShapeModel::build: building shape model");
    _data = data;
    peaks->computeSigmas();
    _params->sigma_d = peaks->sigmaD();
    _params->sigma_m = peaks->sigmaM();
    std::vector<ohkl::Peak3D*> fit_peaks;

    for (ohkl::Peak3D* peak : peaks->getPeakList()) {
        if (!peak->enabled())
            continue;
        const double d = 1.0 / peak->q().rowVector().norm();

        if (d > _params->d_max || d < _params->d_min)
            continue;

        const ohkl::Intensity intensity = peak->correctedSumIntensity();

        if (intensity.value() <= _params->strength_min * intensity.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    ohklLog(Level::Info, "ShapeModel::build: integrating ", fit_peaks.size(), " peaks");
    ShapeIntegrator integrator(
        this, getAABB(), _params->nbins_x, _params->nbins_y, _params->nbins_z);
    ohkl::IntegrationParameters int_params{};
    int_params.peak_end = _params->peak_end;
    int_params.bkg_begin = _params->bkg_begin;
    int_params.bkg_end = _params->bkg_end;
    int_params.fixed_peak_end = _params->fixed_peak_end;
    int_params.fixed_bkg_begin = _params->fixed_bkg_begin;
    int_params.fixed_bkg_end = _params->fixed_bkg_end;
    int_params.region_type = _params->region_type;
    integrator.setParameters(int_params);
    integrator.integrate(fit_peaks, this, data);
    ohklLog(Level::Info, "ShapeModel::build: finished integrating shapes");
    // ohklLog(Level::Info, "ShapeModel::build: updating fit");
    // updateFit(1000);
    ohklLog(Level::Info, "ShapeModel::build: done");
}

void ShapeModel::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

} // namespace ohkl
