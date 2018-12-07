/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <algorithm>
#include <stdexcept>

#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "IDataReader.h"
#include "MillerIndex.h"
#include "Peak3D.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"
#include "Units.h"

namespace nsx {

Peak3D::Peak3D(sptrDataSet data):
    _shape(),
    _unitCell(nullptr),
    _scale(1.0),
    _selected(true),
    _masked(false),
    _predicted(true),
    _transmission(1.0),
    _data(data),
    _rockingCurve(),
    _peakEnd(4.0),
    _bkgBegin(5.0),
    _bkgEnd(6.0)
{

}

Peak3D::Peak3D(sptrDataSet data, const Ellipsoid &shape):
    Peak3D(data)
{
    setShape(shape);
}

void Peak3D::setShape(const Ellipsoid& shape)
{
    // shape should be consistent with data
    if (_data) {
        Eigen::Vector3d c = shape.center();
        if (c[2] < 0.0 || c[2] > _data->nFrames()-1
          || c[0] < 0.0 || c[0] >_data->nCols()-1
          || c[1] < 0.0 || c[1] > _data->nRows()-1) {
            throw std::runtime_error("Peak3D::setShape(): peak center out of bounds");
        }
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

void Peak3D::setUnitCell(sptrUnitCell uc)
{
    _unitCell = uc;
}

sptrUnitCell Peak3D::unitCell() const
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
    auto state = _data->interpolatedState(c[2]);
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

void Peak3D::updateIntegration(const IPeakIntegrator& integrator, double peakEnd, double bkgBegin, double bkgEnd)
{
    _rockingCurve = integrator.rockingCurve();
    // testing
    //_meanBackground = integrator.meanBackground();
    //_rawIntensity = integrator.peakIntensity();
    _meanBackground = integrator.meanBackground();
    _rawIntensity = integrator.integratedIntensity();
    // testing!!
    //_shape = integrator.fitShape();
    _peakEnd = peakEnd;
    _bkgBegin = bkgBegin;
    _bkgEnd = bkgEnd;
}

void Peak3D::setRawIntensity(const Intensity& i)
{
    // note: the scaling factor is taken to be consistent with Peak3D::getRawIntensity()
    _rawIntensity = i; // / data()->getSampleStepSize();
}

ReciprocalVector Peak3D::q() const
{
    auto pixel_coords = _shape.center();
    auto state = _data->interpolatedState(pixel_coords[2]);
    const auto* detector = _data->reader()->diffractometer()->detector();
    auto detector_position = DirectVector(detector->pixelPosition(pixel_coords[0], pixel_coords[1]));
    return state.sampleQ(detector_position);
}

//! This method computes an ellipsoid in q-space which is approximately the transformation from
//! detector space to q-space of its shape ellipsoid (which is computed during blob search).
//!
//! Suppose that the detector-space ellipsoid is given by the equation (x-x0).dot(A*(x-x0)) <= 1.
//! Then if q = q0 + J(x-x0), then the corresponding ellipsoid.
//!
//! This method can throw if there is no valid q-shape corresponding to the detector space shape.
Ellipsoid Peak3D::qShape() const
{
    if (!_data) {
        throw std::runtime_error("Attempted to compute q-shape of peak not attached to data");
    }

    Eigen::Vector3d p = _shape.center();
    auto state = _data->interpolatedState(p[2]);
    Eigen::Vector3d q0 = q().rowVector();

    // Jacobian of map from detector coords to sample q space
    Eigen::Matrix3d J = state.jacobianQ(p[0], p[1]);
    const Eigen::Matrix3d JI = J.inverse();

    // inverse covariance matrix in sample q space
    const Eigen::Matrix3d q_inv_cov = JI.transpose() * _shape.metric() * JI;
    return Ellipsoid(q0, q_inv_cov);
}


ReciprocalVector Peak3D::qPredicted() const
{
    if (!_unitCell) {
        return {};
    }
    auto index = MillerIndex(q(), *_unitCell);
    return ReciprocalVector(_unitCell->fromIndex(index.rowVector().cast<double>()));
}

DetectorEvent Peak3D::predictCenter(double frame) const
{
    const DetectorEvent no_event = {0, 0, -1, -1};

    if (!_unitCell) {
        return no_event;
    }

    auto index = MillerIndex(q(), *_unitCell);
    auto state = _data->interpolatedState(frame);
    Eigen::RowVector3d q_hkl = _unitCell->fromIndex(index.rowVector().cast<double>());
    Eigen::RowVector3d ki = state.ki().rowVector();
    Eigen::RowVector3d kf = q_hkl*state.sampleOrientationMatrix().transpose() + ki;

    const double alpha = ki.norm() / kf.norm();


    Eigen::RowVector3d kf1 = alpha*kf;
    Eigen::RowVector3d kf2 = -alpha*kf;

    Eigen::RowVector3d pred_kf = (kf1-kf).norm() < (kf2-kf).norm() ? kf1 : kf2;

    return _data->reader()->diffractometer()->detector()->constructEvent(DirectVector(state.samplePosition), ReciprocalVector(pred_kf*state.detectorOrientation));
}


Intensity Peak3D::meanBackground() const {
    return _meanBackground;
}

double Peak3D::peakEnd() const {
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

} // end namespace nsx
