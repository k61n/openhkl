//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/ShapeCollection.cpp
//! @brief     Implements classes PeakInterpolation, ShapeCollection
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/ShapeCollection.h"

#include "base/fit/Minimizer.h"
#include "base/geometry/Ellipsoid.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
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

namespace nsx {

void ShapeCollectionParameters::log(const Level& level) const
{
    IntegrationParameters::log(level);
    nsxlog(level, "Shape Collection parameters:");
    nsxlog(level, "d_min     = ", d_min);
    nsxlog(level, "d_max     = ", d_max);
    nsxlog(level, "strength_min           = ", strength_min);
    nsxlog(level, "kabsch_coords          = ", kabsch_coords);
    nsxlog(level, "nbins_x                = ", nbins_x);
    nsxlog(level, "nbins_y                = ", nbins_y);
    nsxlog(level, "nbins_z                = ", nbins_z);
    nsxlog(level, "min_n_neighbors        = ", min_n_neighbors);
    nsxlog(level, "sigma_m                = ", sigma_m);
    nsxlog(level, "sigma_d                = ", sigma_d);
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

//! Helper struct used internally by the shape collection.
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
        const auto state = InterpolatedState::interpolate(peak->dataSet()->instrumentStates(), center[2]);

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

ShapeCollection::ShapeCollection()
    : _profiles()
    , _choleskyD()
    , _choleskyM()
    , _choleskyS()
{
    _choleskyD.fill(1e-6);
    _choleskyM.fill(1e-6);
    _choleskyS.fill(1e-6);
    _params = std::make_shared<ShapeCollectionParameters>();
}

ShapeCollection::ShapeCollection(std::shared_ptr<ShapeCollectionParameters> params)
    : _profiles()
    , _choleskyD()
    , _choleskyM()
    , _choleskyS()
    , _params(params)
{
    _choleskyD.fill(1e-6);
    _choleskyM.fill(1e-6);
    _choleskyS.fill(1e-6);
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

bool ShapeCollection::addPeak(Peak3D* peak, Profile3D&& profile, Profile1D&& integrated_profile)
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

void ShapeCollection::updateFit(int /*num_iterations*/)
{
#if 0 // TODO restore

    std::vector<std::pair<Eigen::Matrix3d, FitData>> fit_data;
    std::vector<std::pair<int, int>> fit_constraints;
    fit_data.reserve(_profiles.size());

    for (const auto& pair: _profiles) {
        auto peak = pair.first;
        Eigen::Matrix3d cov = peak->shape().inverseMetric();
        FitData data(peak);
        fit_data.push_back({0.5*(cov+cov.transpose()), data});
    }

    nsx::FitParameters params;

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

        for (const auto& tup: fit_data) {
            const auto& cov = tup.first;
            const auto& data = tup.second;

            covariance_helper(pred_cov, data, sigmaD, sigmaM, sigmaS);
            Eigen::Matrix3d delta = cov - pred_cov;

            for (int i = 0; i < 3; ++i) {
                for (int j = i; j < 3; ++j)
                    r(k++) = delta(i,j);
            }
        }

        return 0;
    };

    nsx::Minimizer min;
    min.initialize(params, 6*fit_data.size());
    min.set_f(residual);
    min.fit(num_iterations);
#endif
}

void ShapeCollection::setParameters(std::shared_ptr<ShapeCollectionParameters> params)
{
    _params.reset();
    _params = params;
}

Eigen::Matrix3d ShapeCollection::predictCovariance(Peak3D* peak) const
{
    FitData f(peak);
    return predictCovariance(f);
}

Eigen::Matrix3d ShapeCollection::predictCovariance(const FitData& f) const
{
    Eigen::Matrix3d result;
    Eigen::Matrix3d sigmaM = from_cholesky(_choleskyM);
    Eigen::Matrix3d sigmaD = from_cholesky(_choleskyD);
    Eigen::Matrix3d sigmaS = from_cholesky(_choleskyS);
    covariance_helper(result, f, sigmaD, sigmaM, sigmaS);
    return result;
}

double ShapeCollection::meanPearson() const
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

std::optional<Profile3D> ShapeCollection::meanProfile(
    const DetectorEvent& ev, double radius, double nframes) const
{
    Profile3D mean;
    auto neighbors = findNeighbors(ev, radius, nframes);
    if (!neighbors)
        return {};

    for (auto peak : neighbors.value()) {
        // double weight = (1-r/radius) * (1-df/nframes);
        // mean.addProfile(profile, weight*weight);
        mean.addProfile(_profiles.find(peak)->second.first, 1.0);
    }

    mean.normalize();
    return mean;
}

std::optional<std::vector<Intensity>> ShapeCollection::meanProfile1D(
    const DetectorEvent& ev, double radius, double nframes) const
{
    auto neighbors = findNeighbors(ev, radius, nframes);
    if (!neighbors)
        return {};

    std::vector<Intensity> mean_profile;
    const double inv_N = 1.0 / neighbors.value().size();

    for (auto peak : neighbors.value()) {
        const auto& profile = _profiles.find(peak)->second.second.profile();

        if (mean_profile.empty())
            mean_profile.resize(profile.size());

        for (size_t i = 0; i < mean_profile.size(); ++i)
            mean_profile[i] += profile[i] * inv_N;
    }
    return mean_profile;
}

std::optional<std::vector<Peak3D*>> ShapeCollection::findNeighbors(
    const DetectorEvent& ev, double radius, double nframes) const
{
    std::vector<Peak3D*> neighbors;
    Eigen::Vector3d center(ev.px, ev.py, ev.frame);

    for (const auto& pair : _profiles) {
        auto peak = pair.first;
        Eigen::Vector3d dc = center - peak->shape().center();
        // too far away on detector
        if (dc(0) * dc(0) + dc(1) * dc(1) > radius * radius)
            continue;
        // too far away in frame number
        if (std::fabs(dc(2)) > nframes)
            continue;
        neighbors.push_back(peak);
    }
    if (neighbors.empty())
        return {};
    return neighbors;
}

std::optional<Eigen::Matrix3d> ShapeCollection::meanCovariance(
    Peak3D* reference_peak, double radius, double nframes, size_t min_neighbors,
    PeakInterpolation interpolation) const
{
    Eigen::Matrix3d cov;
    cov.setZero();
    auto neighbors =
        findNeighbors(DetectorEvent(reference_peak->shape().center()), radius, nframes);

    if (!neighbors) {
        reference_peak->setRejectionFlag(RejectionFlag::NoNeighbours);
        reference_peak->setSelected(false);
        return {};
    }
    if (neighbors.value().size() < min_neighbors) {
        reference_peak->setRejectionFlag(RejectionFlag::TooFewNeighbours);
        reference_peak->setSelected(false);
        return {};
    }

    PeakCoordinateSystem reference_coord(reference_peak);

    double sum_weight(0.0);
    for (auto peak : neighbors.value()) {
        PeakCoordinateSystem coord(peak);
        Eigen::Matrix3d J = coord.jacobian();

        double weight;
        switch (interpolation) {
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
                auto corrected_intensity = peak->correctedIntensity();
                double intensity = corrected_intensity.value();
                double sigma = corrected_intensity.sigma();
                weight = intensity / sigma;
                break;
            }
            default: {
                reference_peak->setRejectionFlag(RejectionFlag::InterpolationFailure);
                reference_peak->setSelected(false);
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

void ShapeCollection::setPredictedShapes(
    PeakCollection* peaks, PeakInterpolation interpolation, sptrProgressHandler handler)
{
    nsxlog(
        Level::Info, "ShapeCollection: Computing shapes of ", peaks->numberOfPeaks(),
        " calculated peaks");

    int count = 0;
    int npeaks = peaks->numberOfPeaks();
    std::ostringstream oss;
    oss << "Computing shapes of " << npeaks << " peaks";
    if (handler) {
        handler->setStatus(oss.str().c_str());
        handler->setProgress(0);
    }

    #pragma omp parallel for
    for (auto peak : peaks->getPeakList()) {
        peak->setPredicted(true);
        peak->setSelected(true);

        // Skip the peak if any error occur when computing its mean covariance (e.g.
        // too few or no neighbouring peaks found)
        if (auto cov = meanCovariance(
            peak, _params->neighbour_range_pixels, _params->neighbour_range_frames,
            _params->min_n_neighbors, interpolation)) {
            Eigen::Vector3d center = peak->shape().center();
            peak->setShape(Ellipsoid(center, cov.value().inverse()));
        }

        if (handler) {
            double progress = ++count * 100.0 / npeaks;
            handler->setProgress(progress);
        }
    }
    nsxlog(Level::Info, "ShapeCollection: finished computing shapes");
}

std::array<double, 6> ShapeCollection::choleskyD() const
{
    return _choleskyD;
}

std::array<double, 6> ShapeCollection::choleskyM() const
{
    return _choleskyM;
}

std::array<double, 6> ShapeCollection::choleskyS() const
{
    return _choleskyS;
}

std::map<Peak3D*, std::pair<Profile3D, Profile1D>> ShapeCollection::profiles() const
{
    return _profiles;
}

ShapeCollectionParameters* ShapeCollection::parameters()
{
    return _params.get();
}

bool ShapeCollection::detectorCoords() const
{
    return !_params->kabsch_coords;
}

AABB ShapeCollection::getAABB()
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

void ShapeCollection::integrate(
    std::vector<Peak3D*> peaks, std::set<nsx::sptrDataSet> datalist, sptrProgressHandler handler)
{
    nsxlog(Level::Info, "ShapeCollection::integrate: integrating ", peaks.size(), " peaks");
    ShapeIntegrator integrator(
        this, getAABB(), _params->nbins_x, _params->nbins_y, _params->nbins_z);
    nsx::IntegrationParameters int_params{};
    int_params.peak_end = _params->peak_end;
    int_params.bkg_begin = _params->bkg_begin;
    int_params.bkg_end = _params->bkg_end;
    integrator.setHandler(handler);
    integrator.setNNumors(datalist.size());
    integrator.setParameters(int_params);

    int n_numor = 1;
    for (const nsx::sptrDataSet& data : datalist) {
        integrator.integrate(peaks, this, data, n_numor);
        ++n_numor;
    }
    nsxlog(Level::Info, "ShapeCollection::integrate: finished integrating shapes");
}

} // namespace nsx
