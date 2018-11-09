#include <stdexcept>

#include <Eigen/Core>

#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Ellipsoid.h"
#include "Logger.h"
#include "Minimizer.h"
#include "Peak3D.h"
#include "ShapeLibrary.h"
#include "Source.h"
#include "UnitCell.h"

namespace nsx {

static PeakList buildPeaksFromMillerIndices(sptrDataSet data, const std::vector<MillerIndex>& hkls, const Eigen::Matrix3d& BU)
{
    std::vector<ReciprocalVector> qs;
    PeakList peaks;

    for (auto idx: hkls) {
        qs.emplace_back(idx.rowVector().cast<double>()*BU);
    }

    auto events = data->events(qs);

    for (auto event: events) {
        sptrPeak3D peak(new Peak3D(data));
        Eigen::Vector3d center = {event._px, event._py, event._frame};

        // dummy shape
        try {
            peak->setShape(Ellipsoid(center, 1.0));
            peaks.push_back(peak);
        } catch(...) {
            // invalid shape, nothing to do
        }
    }
    return peaks;
}

PeakList predictPeaks(ShapeLibrary library,
                      sptrDataSet data,
                      sptrUnitCell unit_cell,
                      double dmin,
                      double dmax,
                      double radius,
                      double nframes,
                      int min_neighbors,
                      PeakInterpolation interpolation)
{
    // Generate the Miller indices found in the [dmin,dmax] shell
    const auto& mono = data->diffractometer()->source().selectedMonochromator();
    const double wavelength = mono.wavelength();
    auto predicted_hkls = unit_cell->generateReflectionsInShell(dmin, dmax, wavelength);

    PeakList peaks = buildPeaksFromMillerIndices(data, predicted_hkls, unit_cell->reciprocalBasis());
    nsx::info() << "Computing shapes of " << peaks.size() << " calculated peaks...";

    PeakList predicted_peaks;

    for (auto peak : peaks) {
        peak->setUnitCell(unit_cell);
        peak->setPredicted(true);
        peak->setSelected(true);

        // Skip the peak if any error occur when computing its mean covariance (e.g. too few or no neighbouring peaks found)
        try {
            Eigen::Matrix3d cov = library.meanCovariance(peak, radius, nframes, min_neighbors, interpolation);
            //Eigen::Matrix3d cov = _library->predictCovariance(p);
            Eigen::Vector3d center = peak->shape().center();
            peak->setShape(Ellipsoid(center, cov.inverse()));
        } catch (std::exception& e) {
            nsx::info() << e.what();
            continue;
        }
        predicted_peaks.push_back(peak);
    }
    return predicted_peaks;
}

static Eigen::Matrix3d from_cholesky(const std::array<double, 6>& components)
{
    Eigen::Matrix3d L;
    L.setZero();
    L(0,0) = components[0];
    L(1,1) = components[1];
    L(2,2) = components[2];
    L(1,0) = components[3];
    L(2,0) = components[4];
    L(2,1) = components[5];

    return L*L.transpose();
}

//! \brief Helper struct used internally by the shape library.
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
    FitData(sptrPeak3D peak)
    {
        const auto* detector = peak->data()->diffractometer()->detector();
        Eigen::Vector3d center = peak->shape().center();
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
        Jd = state.jacobianQ(event._px, event._py);

        q = kf-ki;
    }
};


ShapeLibrary::ShapeLibrary(bool detector_coords, double peakScale, double bkgBegin, double bkgEnd): 
    _profiles(),
    _choleskyD(),
    _choleskyM(),
    _choleskyS(),
    _detectorCoords(detector_coords),
    _peakScale(peakScale),
    _bkgBegin(bkgBegin),
    _bkgEnd(bkgEnd)
{
    _choleskyD.fill(1e-6);
    _choleskyM.fill(1e-6);
    _choleskyS.fill(1e-6);
}

static void covariance_helper(Eigen::Matrix3d& result, const FitData& f, const Eigen::Matrix3d& sigmaD, const Eigen::Matrix3d& sigmaM, const Eigen::Matrix3d& sigmaS)
{
    static constexpr double deg2 = (M_PI/180)*(M_PI/180);
    Eigen::Matrix3d E;
    E.setIdentity();

    Eigen::Matrix3d cov;
    cov.setZero();

    Eigen::Matrix3d Jd = f.Jd.inverse();
    Eigen::Matrix3d Jk = f.Rs * f.Jk;
    Eigen::Matrix3d Jp = f.Rs * f.Jp;
    Eigen::Vector3d qs = f.Rs*f.q;

    Eigen::Matrix3d P = f.q.squaredNorm() * Eigen::Matrix3d::Identity() - qs*qs.transpose();

    // mosaicity
    cov += P * sigmaM * P.transpose();
    // beam divergence
    cov += f.ki.squaredNorm() * Jk * sigmaD * Jk.transpose();
    // shape
    cov += Jp * sigmaS * Jp.transpose();

    result = Jd * cov * Jd.transpose();
}

double ShapeLibrary::peakScale() const
{
    return _peakScale;
}


double ShapeLibrary::bkgBegin() const
{
    return _bkgBegin;
}

double ShapeLibrary::bkgEnd() const
{
    return _bkgEnd;
}

bool ShapeLibrary::addPeak(sptrPeak3D peak, Profile3D&& profile, Profile1D&& integrated_profile)
{
    Eigen::Matrix3d A = peak->shape().inverseMetric();
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
#if 0

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
#endif
}

Eigen::Matrix3d ShapeLibrary::predictCovariance(sptrPeak3D peak) const
{
    FitData f(peak);
    return predictCovariance(f);
}

Eigen::Matrix3d ShapeLibrary::predictCovariance(const FitData& f) const
{
    Eigen::Matrix3d result;
    Eigen::Matrix3d sigmaM = from_cholesky(_choleskyM);
    Eigen::Matrix3d sigmaD = from_cholesky(_choleskyD);
    Eigen::Matrix3d sigmaS = from_cholesky(_choleskyS);
    covariance_helper(result, f, sigmaD, sigmaM, sigmaS);
    return result;
}

double ShapeLibrary::meanPearson() const
{
    double sum_pearson = 0;

    for (const auto& pair: _profiles) {
        auto peak = pair.first;
        Eigen::Matrix3d obs_cov = peak->shape().inverseMetric();
        Eigen::Matrix3d pred_cov = predictCovariance(peak);
        sum_pearson += (obs_cov.transpose()*pred_cov).trace() / obs_cov.norm() / pred_cov.norm();
    }
    return sum_pearson / _profiles.size();
}

Profile3D ShapeLibrary::meanProfile(const DetectorEvent& ev, double radius, double nframes) const
{
    Profile3D mean;
    PeakList neighbors = findNeighbors(ev, radius, nframes);

    for (auto peak: neighbors) {
        //double weight = (1-r/radius) * (1-df/nframes);
        // mean.addProfile(profile, weight*weight);
        mean.addProfile(_profiles.find(peak)->second.first, 1.0);
    }

    mean.normalize();
    return mean;
}

std::vector<Intensity> ShapeLibrary::meanProfile1D(const DetectorEvent& ev, double radius, double nframes) const
{
    PeakList neighbors = findNeighbors(ev, radius, nframes);
    std::vector<Intensity> mean_profile;
    const double inv_N = 1.0 / neighbors.size();

    for (auto peak: neighbors) {
        const auto& profile = _profiles.find(peak)->second.second.profile();

        if (mean_profile.size() == 0) {
            mean_profile.resize(profile.size());
        }

        for (size_t i = 0; i < mean_profile.size(); ++i) {
            mean_profile[i] += profile[i] * inv_N;
        }
    }
    return mean_profile;
}

PeakList ShapeLibrary::findNeighbors(const DetectorEvent& ev, double radius, double nframes) const
{
    PeakList neighbors;
    Eigen::Vector3d center(ev._px, ev._py, ev._frame);
    
    for (const auto& pair: _profiles) {
        auto peak = pair.first;    
        Eigen::Vector3d dc = center - peak->shape().center();
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

Eigen::Matrix3d ShapeLibrary::meanCovariance(sptrPeak3D reference_peak, double radius, double nframes, size_t min_neighbors, PeakInterpolation interpolation) const
{
    Eigen::Matrix3d cov;
    cov.setZero();
    PeakList neighbors = findNeighbors(DetectorEvent(reference_peak->shape().center()), radius, nframes);

    if (neighbors.empty() || (neighbors.size() < min_neighbors)) {
        throw std::runtime_error("ShapeLibrary::meanCovariance(): peak has no or too few neighbors");
    }

    PeakCoordinateSystem reference_coord(reference_peak);

    double sum_weight(0.0);
    for (auto peak: neighbors) {
        PeakCoordinateSystem coord(peak);
        Eigen::Matrix3d J = coord.jacobian();

        double weight;
        switch(interpolation) {
        case(PeakInterpolation::NoInterpolation): {
            weight = 1.0;
            break;
        }
        case(PeakInterpolation::InverseDistance): {
            Eigen::RowVector3d dq = reference_peak->q().rowVector() - peak->q().rowVector();
            weight = 1.0/dq.norm();
            break;
        }
        case(PeakInterpolation::Intensity): {
            auto corrected_intensity = peak->correctedIntensity();
            double intensity = corrected_intensity.value();
            double sigma = corrected_intensity.sigma();
            weight = intensity/sigma;
            break;
        }
        default: {
            throw std::runtime_error("Invalid peak interpolation");
        }
        }

        cov += weight*(J * peak->shape().inverseMetric() * J.transpose());

        sum_weight += weight;
    }

    cov /= sum_weight;

    Eigen::Matrix3d JI = reference_coord.jacobian().inverse();
    return JI * cov * JI.transpose();
}

bool ShapeLibrary::detectorCoords() const
{
    return _detectorCoords;
}

} // end namespace nsx
