//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/Peak3D.cpp
//! @brief     Implements class Peak3D
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/Peak3D.h"

#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/IDataReader.h"
#include "tables/crystal/MillerIndex.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nsx {

Peak3D::Peak3D(sptrDataSet data)
    : _shape()
    , _peakEnd(4.0)
    , _bkgBegin(5.0)
    , _bkgEnd(6.0)
    , _unitCell(nullptr)
    , _scale(1.0)
    , _selected(true)
    , _masked(false)
    , _predicted(true)
    , _caught_by_filter(false)
    , _rejected_by_filter(false)
    , _transmission(1.0)
    , _data(data)
    , _rockingCurve()
{
}

Peak3D::Peak3D(sptrDataSet data, const Ellipsoid& shape) : Peak3D(data)
{
    setShape(shape);
}

Peak3D::Peak3D(std::shared_ptr<nsx::Peak3D> peak)
{
    setShape(peak->shape());
    _peakEnd = peak->peakEnd();
    _bkgBegin = peak->bkgBegin();
    _bkgEnd = peak->bkgEnd();
    _unitCell = peak->unitCell();
    _scale = peak->scale();
    _selected = peak->selected();
    _masked = peak->masked();
    _predicted = peak->predicted();
    _transmission = peak->transmission();
    _data = peak->dataSet();
    _rockingCurve = peak->rockingCurve();
    _meanBackground = peak->meanBackground();
    _rawIntensity = peak->rawIntensity();

    _caught_by_filter = false;
    _rejected_by_filter = false;
}

void Peak3D::setShape(const Ellipsoid& shape)
{
    // TODO: restore this assertion elsewhere
    //    // shape should be consistent with data
    //    if (_data) {
    //        Eigen::Vector3d c = shape.center();
    //        if (c[2] < 0.0 || c[2] > _data->nFrames() - 1 || c[0] < 0.0 || c[0] > _data->nCols() -
    //        1
    //            || c[1] < 0.0 || c[1] > _data->nRows() - 1) {
    //            throw std::runtime_error("Peak3D::setShape(): peak center out of bounds");
    //        }
    //    }
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

void Peak3D::setUnitCell(const UnitCell* uc)
{
    _unitCell = uc;
}

const UnitCell* Peak3D::unitCell() const
{
    return _unitCell;
}

Intensity Peak3D::rawIntensity() const
{
    return _rawIntensity;
}

Intensity Peak3D::correctedIntensity() const
{
    auto c = _shape.center();
    auto state = _data->instrumentStates().interpolate(c[2]);
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
}

bool Peak3D::selected() const
{
    return _selected;
}

void Peak3D::setMasked(bool masked)
{
    _masked = masked;
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
    const Intensity& integratedIntensity, double peakEnd, double bkgBegin, double bkgEnd)
{
    _rockingCurve = rockingCurve;
    _meanBackground = meanBackground;
    _rawIntensity = integratedIntensity;
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
    auto pixel_coords = _shape.center();
    auto state = _data->instrumentStates().interpolate(pixel_coords[2]);
    const auto* detector = _data->reader()->diffractometer()->detector();
    auto detector_position =
        DirectVector(detector->pixelPosition(pixel_coords[0], pixel_coords[1]));
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
    auto state = _data->instrumentStates().interpolate(p[2]);
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
    double transmission, Intensity mean_bkg, bool predicted, bool selected, bool masked)
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
}


Intensity Peak3D::meanBackground() const
{
    return _meanBackground;
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
    try {
        _hkl = MillerIndex(q(), *_unitCell);
    } catch(std::range_error& e) {
        _selected = false;
    }
}

} // namespace nsx
