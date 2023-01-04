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
    {RejectionFlag::Outlier, "Rejected by outlier detection algorithm"}};

Peak3D::Peak3D(sptrDataSet data)
    : _shape()
    , _meanBackground()
    , _meanBkgGradient()
    , _peakEnd(3.0)
    , _bkgBegin(3.0)
    , _bkgEnd(6.0)
    , _scale(1.0)
    , _selected(true)
    , _masked(false)
    , _predicted(true)
    , _caught_by_filter(false)
    , _rejected_by_filter(false)
    , _transmission(1.0)
    , _rejection_flag(RejectionFlag::NotRejected)
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
    , _meanBackground()
    , _meanBkgGradient()
    , _peakEnd(3.0)
    , _bkgBegin(3.0)
    , _bkgEnd(6.0)
    , _hkl(hkl)
    , _scale(1.0)
    , _selected(true)
    , _masked(false)
    , _predicted(true)
    , _caught_by_filter(false)
    , _rejected_by_filter(false)
    , _transmission(1.0)
    , _rejection_flag(RejectionFlag::NotRejected)
    , _data(data)
    , _rockingCurve()
{
}

Peak3D::Peak3D(std::shared_ptr<ohkl::Peak3D> peak)
{
    setShape(peak->shape());
    _meanBackground = peak->meanBackground();
    _meanBkgGradient = peak->meanBkgGradient();
    _peakEnd = peak->peakEnd();
    _bkgBegin = peak->bkgBegin();
    _bkgEnd = peak->bkgEnd();
    _unitCell = peak->_unitCell;
    _scale = peak->scale();
    _selected = peak->selected();
    _masked = peak->masked();
    _predicted = peak->predicted();
    _transmission = peak->transmission();
    _data = peak->dataSet();
    _rockingCurve = peak->rockingCurve();
    _rawIntensity = peak->rawIntensity();
    _hkl = peak->hkl();

    _caught_by_filter = false;
    _rejected_by_filter = false;
}

void Peak3D::setShape(const Ellipsoid& shape)
{
    // shape should be consistent with data
    if (_data) {
        Eigen::Vector3d c = shape.center();
        if (c[2] < 0.0 || c[2] > _data->nFrames() - 1 || c[0] < 0.0 || c[0] > _data->nCols() - 1
            || c[1] < 0.0 || c[1] > _data->nRows() - 1) {
            setSelected(false);
            setRejectionFlag(RejectionFlag::OutsideFrames);
        }
    }
    _shape = shape;
}

Ellipsoid Peak3D::shape() const
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

Intensity Peak3D::rawIntensity() const
{
    return _rawIntensity;
}

Intensity Peak3D::correctedIntensity() const
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
    return rawIntensity() * factor / state.stepSize;
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
    return (!_masked && _selected);
}

void Peak3D::setSelected(bool s)
{
    _selected = s;
    if (s)
        setRejectionFlag(RejectionFlag::NotRejected, true);
}

void Peak3D::reject(RejectionFlag flag)
{
    _selected = false;
    if (flag == RejectionFlag::NotRejected)
        _rejection_flag = flag;
}

bool Peak3D::selected() const
{
    return _selected;
}

void Peak3D::setMasked(bool masked)
{
    _masked = masked;
    if (_masked)
        _rejection_flag = RejectionFlag::Masked;
    else
        _rejection_flag = RejectionFlag::NotRejected;
}

bool Peak3D::masked() const
{
    return _masked;
}

void Peak3D::setPredicted(bool predicted)
{
    _predicted = predicted;
}

bool Peak3D::predicted() const
{
    return _predicted;
}

void Peak3D::updateIntegration(
    const std::vector<Intensity>& rockingCurve, const Intensity& meanBackground,
    const Intensity& meanBkgGradient, const Intensity& integratedIntensity, double peakEnd,
    double bkgBegin, double bkgEnd)
{
    _rockingCurve = rockingCurve;
    _meanBackground = meanBackground;
    _meanBkgGradient = meanBkgGradient;
    _rawIntensity = integratedIntensity;

    if (_rawIntensity.sigma() < _sigma2_eps) { // NaN sigma handled by Intensity constructor
        setSelected(false);
        setRejectionFlag(RejectionFlag::InvalidSigma, true);
    }
    if (_meanBackground.sigma() < _sigma2_eps) { // NaN sigma handled by Intensity constructor
        setSelected(false);
        setRejectionFlag(RejectionFlag::InvalidBkgSigma, true);
    }

    //_rawIntensity = integrator.peakIntensity(); // TODO: test, reactivate ???
    //_shape = integrator.fitShape(); // TODO: test, reactivate ???
    _peakEnd = peakEnd;
    _bkgBegin = bkgBegin;
    _bkgEnd = bkgEnd;
}

void Peak3D::setRawIntensity(const Intensity& i)
{
    // note: the scaling factor is taken to be consistent with Peak3D::getRawIntensity()
    _rawIntensity = i; // TODO: restore normalization ??: / data()->getSampleStepSize();
}

ReciprocalVector Peak3D::q() const
{
    auto state = InterpolatedState::interpolate(_data->instrumentStates(), _shape.center()[2]);
    if (!state.isValid()) // this is the source of many interpolation problems
        return ReciprocalVector();
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

// found unused except in TestQShape (JWu 11jun19)

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
    Intensity intensity, double peakEnd, double bkgBegin, double bkgEnd, double scale,
    double transmission, Intensity mean_bkg, bool predicted, bool selected, bool masked,
    int rejection_flag, int integration_flag, Intensity mean_bkg_grad /* = {} */)
{
    _peakEnd = peakEnd;
    _bkgBegin = bkgBegin;
    _bkgEnd = bkgEnd;
    _scale = scale;
    _selected = selected;
    _masked = masked;
    _predicted = predicted;
    _transmission = transmission;
    _meanBackground = mean_bkg;
    _rawIntensity = intensity;
    _rejection_flag = static_cast<RejectionFlag>(rejection_flag);
    _integration_flag = static_cast<RejectionFlag>(integration_flag);
    _meanBkgGradient = mean_bkg_grad;
}


Intensity Peak3D::meanBackground() const
{
    return _meanBackground;
}

Intensity Peak3D::meanBkgGradient() const
{
    return _meanBkgGradient;
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
            _selected = false;
            _rejection_flag = RejectionFlag::InterpolationFailure;
        }
    }
}

void Peak3D::setRejectionFlag(RejectionFlag flag, bool overwrite /* = false */)
{
    if (_rejection_flag == RejectionFlag::NotRejected) {
        _rejection_flag = flag;
    } else {
        if (overwrite)
            _rejection_flag = flag;
    }
}

RejectionFlag Peak3D::rejectionFlag() const
{
    if (_integration_flag != RejectionFlag::NotRejected)
        return _integration_flag;
    return _rejection_flag;
}

std::string Peak3D::rejectionString() const
{
    return _rejection_map.find(_rejection_flag)->second;
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
        << std::setprecision(2) << correctedIntensity().value() << std::fixed << std::setw(10)
        << std::setprecision(2) << correctedIntensity().sigma();
    return oss.str();
}

const std::map<RejectionFlag, std::string>& Peak3D::rejectionMap()
{
    return _rejection_map;
}

} // namespace ohkl
