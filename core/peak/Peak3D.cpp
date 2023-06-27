//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/Peak3D.cpp
//! @brief     Implements class Peak3D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/Peak3D.h"

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/LogLevel.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h"
#include "core/integration/IIntegrator.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Intensity.h"
#include "tables/crystal/MillerIndex.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <stdexcept>

namespace ohkl {

const std::map<RejectionFlag, std::string> Peak3D::_rejection_map{
    {RejectionFlag::NotRejected, "Not rejected"},
    {RejectionFlag::Masked, "Masked by user"},
    {RejectionFlag::OutsideThreshold, "Too many or few detector counts"},
    {RejectionFlag::OutsideFrames, "Peak centre outside frame range"},
    {RejectionFlag::OutsideDetector, "Peak centre outside detector image"},
    {RejectionFlag::IntegrationFailure, "Integration failed"},
    {RejectionFlag::TooFewPoints, "Too few points to integrate"},
    {RejectionFlag::NoNeighbours, "No neighbouring profiles to compute shape"},
    {RejectionFlag::TooFewNeighbours, "Too few neighbouring profiles to compute shape"},
    {RejectionFlag::NoUnitCell, "No unit cell assigned"},
    {RejectionFlag::NoDataSet, "No associated data set"},
    {RejectionFlag::InvalidRegion, "Integration region extends beyond image/frame range"},
    {RejectionFlag::InterpolationFailure, "Frame coordinate interpolation failed"},
    {RejectionFlag::InvalidSigma, "Negative, zero, or undefined sigma"},
    {RejectionFlag::InvalidBkgSigma, "Negative, zero, or undefined background sigma"},
    {RejectionFlag::SaturatedPixel, "Peak contains saturated pixel(s)"},
    {RejectionFlag::OverlappingBkg, "Adjacent peak background region overlaps this peak"},
    {RejectionFlag::OverlappingPeak, "Adjacent peak intensity region overlaps this peak"},
    {RejectionFlag::InvalidCentroid, "Centre of mass of peak is inconsistent"},
    {RejectionFlag::InvalidCovariance, "Covariance matrix of peak is inconsistent"},
    {RejectionFlag::InvalidShape, "Shape of peak is too small or large"},
    {RejectionFlag::CentreOutOfBounds, "Peak centre moved beyond bounds of data set"},
    {RejectionFlag::BadIntegrationFit, "Pearson coefficient of fit is too low"},
    {RejectionFlag::NoShapeModel, "No shape model found"},
    {RejectionFlag::NoISigmaMinimum, "Failed to find minimum of I/Sigma"},
    {RejectionFlag::PredictionUpdateFailure, "Failure updating prediction post-refinement"},
    {RejectionFlag::ManuallyRejected, "Manually unselected by user"},
    {RejectionFlag::OutsideIndexingTol, "Outside indexing tolerance"},
    {RejectionFlag::Outlier, "Rejected by outlier detection algorithm"},
    {RejectionFlag::Extinct, "Extinct from space group"}};

Peak3D::Peak3D(sptrDataSet data)
    : _shape()
    , _peakEnd(3.0)
    , _bkgBegin(3.0)
    , _bkgEnd(6.0)
    , _regionType(RegionType::VariableEllipsoid)
    , _scale(1.0)
    , _caught_by_filter(false)
    , _rejected_by_filter(false)
    , _transmission(1.0)
    , _rejection_flag(RejectionFlag::NotRejected)
    , _sum_integration_flag(RejectionFlag::NotRejected)
    , _profile_integration_flag(RejectionFlag::NotRejected)
    , _data(data)
    , _rockingCurve()
{
}

Peak3D::Peak3D(sptrDataSet data, const Ellipsoid& shape) : Peak3D(data)
{
    setShape(shape);
}

Peak3D::Peak3D(sptrDataSet data, const MillerIndex& hkl)
    : _shape()
    , _peakEnd(3.0)
    , _bkgBegin(3.0)
    , _bkgEnd(6.0)
    , _regionType(RegionType::VariableEllipsoid)
    , _hkl(hkl)
    , _scale(1.0)
    , _caught_by_filter(false)
    , _rejected_by_filter(false)
    , _transmission(1.0)
    , _rejection_flag(RejectionFlag::NotRejected)
    , _sum_integration_flag(RejectionFlag::NotRejected)
    , _profile_integration_flag(RejectionFlag::NotRejected)
    , _data(data)
    , _rockingCurve()
{
}

Peak3D::Peak3D(std::shared_ptr<ohkl::Peak3D> peak)
{
    setShape(peak->shape());
    _sumIntensity = peak->sumIntensity();
    _profileIntensity = peak->profileIntensity();
    _sumBackground = peak->sumBackground();
    _profileBackground = peak->profileBackground();
    _meanBkgGradient = peak->meanBkgGradient();
    _peakEnd = peak->peakEnd();
    _bkgBegin = peak->bkgBegin();
    _bkgEnd = peak->bkgEnd();
    _regionType = peak->regionType();
    _unitCell = peak->_unitCell;
    _scale = peak->scale();
    _transmission = peak->transmission();
    _data = peak->dataSet();
    _rockingCurve = peak->rockingCurve();
    _hkl = peak->hkl();

    _caught_by_filter = false;
    _rejected_by_filter = false;
}

void Peak3D::setShape(const Ellipsoid& shape)
{
    // shape should be consistent with data
    if (_data) {
        Eigen::Vector3d c = shape.center();
        if (c[2] < 0.0 || c[2] > _data->nFrames() - 1)
            setRejectionFlag(RejectionFlag::OutsideFrames);
        else if (c[0] < 0.0 || c[0] > _data->nCols() - 1 || c[1] < 0.0 || c[1] > _data->nRows() - 1)
            setRejectionFlag(RejectionFlag::OutsideDetector);
    }
    _shape = shape;
}

const Ellipsoid& Peak3D::shape() const
{
    return _shape;
}

const std::vector<Intensity>& Peak3D::rockingCurve() const
{
    return _rockingCurve;
}

void Peak3D::setUnitCell(const sptrUnitCell& uc)
{
    _unitCell = uc;
}

const UnitCell* Peak3D::unitCell() const
{
    if (auto uc = _unitCell.lock())
        return uc.get();
    else
        return nullptr;
}

Intensity Peak3D::correctedIntensity(const Intensity& intensity) const
{
    auto c = _shape.center();
    auto state = InterpolatedState::interpolate(_data->instrumentStates(), c[2]);
    if (!state.isValid()) // Interpolation error
        return Intensity();

    auto diff = state.diffractometer();
    if (diff == nullptr) {
        return Intensity();
    }

    auto detector = diff->detector();
    if (detector == nullptr) {
        return Intensity();
    }

    const double lorentz = state.lorentzFactor(c[0], c[1]);
    const double factor = _scale / lorentz / _transmission;
    return intensity * factor / state.stepSize;
}

Intensity Peak3D::correctedSumIntensity() const
{
    return correctedIntensity(_sumIntensity);
}

Intensity Peak3D::correctedProfileIntensity() const
{
    return correctedIntensity(_profileIntensity);
}

double Peak3D::transmission() const
{
    return _transmission;
}

double Peak3D::scale() const
{
    return _scale;
}

void Peak3D::setScale(double factor)
{
    _scale = factor;
}

void Peak3D::setTransmission(double transmission)
{
    _transmission = transmission;
}

bool Peak3D::enabled() const
{
    return (
        _rejection_flag == RejectionFlag::NotRejected
        && _sum_integration_flag == RejectionFlag::NotRejected
        && _profile_integration_flag == RejectionFlag::NotRejected);
}

void Peak3D::reject(RejectionFlag flag)
{
    if (flag == RejectionFlag::NotRejected)
        _rejection_flag = flag;
}

void Peak3D::updateIntegration(
    const std::vector<Intensity>& rockingCurve, const Intensity& sumBkg, const Intensity& profBkg,
    const Intensity& meanBkgGradient, const Intensity& sumInt, const Intensity& profInt,
    double peakEnd, double bkgBegin, double bkgEnd, const RegionType& regionType)
{
    _rockingCurve = rockingCurve;
    _meanBkgGradient = meanBkgGradient;
    if (sumBkg.isValid()) {
        if (sumBkg.sigma() < _sigma2_eps) // NaN sigma handled by Intensity constructor
            setIntegrationFlag(RejectionFlag::InvalidBkgSigma, IntegratorType::PixelSum);
        else
            _sumBackground = sumBkg;
    }
    if (sumInt.isValid()) { // Default intensity constructor is zero, _valid = false
        if (sumInt.sigma() < _sigma2_eps) // NaN sigma handled by Intensity constructor
            setIntegrationFlag(RejectionFlag::InvalidSigma, IntegratorType::PixelSum);
        else
            _sumIntensity = sumInt;
    }
    if (profInt.isValid()) {
        if (profInt.sigma() < _sigma2_eps) // NaN sigma handled by Intensity constructor
            setIntegrationFlag(RejectionFlag::InvalidSigma, IntegratorType::Profile3D);
        else
            _profileIntensity = profInt;
    }
    if (profBkg.isValid()) {
        if (profBkg.sigma() < _sigma2_eps) // NaN sigma handled by Intensity constructor
            // Not necessarily Profile3D, just establishing *any* profile integration
            setIntegrationFlag(RejectionFlag::InvalidBkgSigma, IntegratorType::Profile3D);
        else
            _profileBackground = profBkg;
    }

    //_sumIntensity = integrator.peakIntensity(); // TODO: test, reactivate ???
    //_shape = integrator.fitShape(); // TODO: test, reactivate ???
    _peakEnd = peakEnd;
    _bkgBegin = bkgBegin;
    _bkgEnd = bkgEnd;
    _regionType = regionType;
}

ReciprocalVector Peak3D::q() const
{
    auto state = InterpolatedState::interpolate(_data->instrumentStates(), _shape.center()[2]);
    if (!state.isValid()) // this is the source of many interpolation problems
        return {};
    return q(state);
}

ReciprocalVector Peak3D::q(const InstrumentState& state) const
{
    const auto* detector = _data->diffractometer()->detector();
    auto detector_position =
        DirectVector(detector->pixelPosition(_shape.center()[0], _shape.center()[1]));
    return state.sampleQ(detector_position);
}

//! This method computes an ellipsoid in q-space which is approximately the
//! transformation from detector space to q-space of its shape ellipsoid (which
//! is computed during blob search).
//!
//! Suppose that the detector-space ellipsoid is given by the equation
//! (x-x0).dot(A*(x-x0)) <= 1. Then if q = q0 + J(x-x0), then the corresponding
//! ellipsoid.
//!
//! This method can throw if there is no valid q-shape corresponding to the
//! detector space shape.
Ellipsoid Peak3D::qShape() const
{
    if (!_data)
        throw std::runtime_error("Attempted to compute q-shape of peak not attached to data");

    Eigen::Vector3d p = _shape.center();
    auto state = InterpolatedState::interpolate(_data->instrumentStates(), p[2]);
    if (!state.isValid()) {
        throw std::range_error("Interpolation error");
    }

    Eigen::Vector3d q0 = q().rowVector();

    // Jacobian of map from detector coords to sample q space
    Eigen::Matrix3d J = state.jacobianQ(p[0], p[1]);
    const Eigen::Matrix3d JI = J.inverse();

    // inverse covariance matrix in sample q space
    const Eigen::Matrix3d q_inv_cov = JI.transpose() * _shape.metric() * JI;
    return Ellipsoid(q0, q_inv_cov);
}

bool Peak3D::caughtByFilter() const
{
    if (_rejected_by_filter)
        return false;
    return _caught_by_filter;
}

bool Peak3D::rejectedByFilter() const
{
    return _rejected_by_filter;
}

void Peak3D::caughtYou(bool caught)
{
    _caught_by_filter = caught;
}

void Peak3D::rejectYou(bool reject)
{
    _rejected_by_filter = reject;
}

void Peak3D::setManually(
    const Intensity& sumInt, const Intensity& profInt, double peakEnd, double bkgBegin,
    double bkgEnd, int region_type, double scale, double transmission, const Intensity& sumBkg,
    const Intensity& profBkg, int rejection_flag, int sum_integration_flag,
    int profile_integration_flag, Intensity sumBkgGrad /* = {} */)
{
    _peakEnd = peakEnd;
    _bkgBegin = bkgBegin;
    _bkgEnd = bkgEnd;
    _regionType = static_cast<RegionType>(region_type);
    _scale = scale;
    _transmission = transmission;
    _sumBackground = sumBkg;
    _profileBackground = profBkg;
    _sumIntensity = sumInt;
    _profileIntensity = profInt;
    _rejection_flag = static_cast<RejectionFlag>(rejection_flag);
    _sum_integration_flag = static_cast<RejectionFlag>(sum_integration_flag);
    _profile_integration_flag = static_cast<RejectionFlag>(profile_integration_flag);
    _meanBkgGradient = sumBkgGrad;
}

double Peak3D::peakEnd() const
{
    return _peakEnd;
}

double Peak3D::bkgBegin() const
{
    return _bkgBegin;
}

double Peak3D::bkgEnd() const
{
    return _bkgEnd;
}

const MillerIndex& Peak3D::hkl() const
{
    return _hkl;
}

void Peak3D::setMillerIndices()
{
    if (unitCell()) {
        ReciprocalVector rv = q();
        if (rv.isValid()) {
            _hkl = MillerIndex(q(), *unitCell());
        } else {
            _hkl = {0, 0, 0};
            _rejection_flag = RejectionFlag::InterpolationFailure;
        }
    }
}

void Peak3D::setRejectionFlag(const RejectionFlag& flag, bool overwrite /* = false */)
{
    if (_rejection_flag == RejectionFlag::NotRejected) {
        _rejection_flag = flag;
    } else {
        if (overwrite)
            _rejection_flag = flag;
    }
}

void Peak3D::setIntegrationFlag(
    const RejectionFlag& flag, const IntegratorType& integrator, bool overwrite /* = false */)
{
    if (integrator == IntegratorType::PixelSum) {
        if (_sum_integration_flag == RejectionFlag::NotRejected) {
            _sum_integration_flag = flag;
        } else {
            if (overwrite)
                _sum_integration_flag = flag;
        }
    } else {
        if (_profile_integration_flag == RejectionFlag::NotRejected) {
            _profile_integration_flag = flag;
        } else {
            if (overwrite)
                _profile_integration_flag = flag;
        }
    }
}

RejectionFlag Peak3D::rejectionFlag() const
{
    if (_profile_integration_flag != RejectionFlag::NotRejected)
        return _profile_integration_flag;
    if (_sum_integration_flag != RejectionFlag::NotRejected)
        return _sum_integration_flag;
    return _rejection_flag;
}

RejectionFlag Peak3D::sumRejectionFlag() const
{
    if (_sum_integration_flag == RejectionFlag::NotRejected)
        return _rejection_flag;
    return _sum_integration_flag;
}

RejectionFlag Peak3D::profileRejectionFlag() const
{
    if (_profile_integration_flag == RejectionFlag::NotRejected)
        return _rejection_flag;
    return _profile_integration_flag;
}

bool Peak3D::isRejectedFor(const RejectionFlag& flag) const
{
    if (_rejection_flag == flag)
        return true;
    if (_sum_integration_flag == flag)
        return true;
    if (_profile_integration_flag == flag)
        return true;
    return false;
}

std::string Peak3D::rejectionString() const
{
    return _rejection_map.find(rejectionFlag())->second;
}

std::string Peak3D::toString() const
{
    std::ostringstream oss;
    // h, k, l, x, y, frame, intensity, sigma
    oss << std::fixed << std::setw(5) << _hkl.h() << std::fixed << std::setw(5) << _hkl.k()
        << std::fixed << std::setw(5) << _hkl.l() << std::fixed << std::setw(10)
        << std::setprecision(2) << shape().center()[0] << std::fixed << std::setw(10)
        << std::setprecision(2) << shape().center()[1] << std::fixed << std::setw(10)
        << std::setprecision(2) << shape().center()[2] << std::fixed << std::setw(10)
        << std::setprecision(2) << correctedSumIntensity().value() << std::fixed << std::setw(10)
        << std::setprecision(2) << correctedSumIntensity().sigma();
    return oss.str();
}

const std::map<RejectionFlag, std::string>& Peak3D::rejectionMap()
{
    return _rejection_map;
}

} // namespace ohkl
