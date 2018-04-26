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
#include <cmath>
#include <stdexcept>

#include "Blob3D.h"
#include "ComponentState.h"
#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "GeometryTypes.h"
#include "Gonio.h"
#include "IFrameIterator.h"
#include "InstrumentState.h"
#include "IPeakIntegrator.h"
#include "MillerIndex.h"
#include "Peak3D.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "Source.h"
#include "UnitCell.h"
#include "Units.h"

namespace nsx {

Peak3D::Peak3D(sptrDataSet data):
    _shape(),
    _unitCells(),
    _scale(1.0),
    _selected(true),
    _masked(false),
    _predicted(true),
    _transmission(1.0),
    _activeUnitCellIndex(0),
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

void Peak3D::setShape(const Ellipsoid& peak)
{
    _shape = peak;
}

const std::vector<Intensity>& Peak3D::rockingCurve() const
{
    return _rockingCurve;
}

void Peak3D::addUnitCell(sptrUnitCell uc, bool activate)
{
    auto it = std::find(_unitCells.begin(),_unitCells.end(), uc);

    if (it != _unitCells.end()) {
        return;
    }
    _unitCells.emplace_back(std::move(uc));

    if (activate) {
        _activeUnitCellIndex = int(_unitCells.size())-1;
    }
}

sptrUnitCell Peak3D::activeUnitCell() const
{
    if (_activeUnitCellIndex < 0 || _activeUnitCellIndex >= int(_unitCells.size())) {
        return nullptr;
    }
    return _unitCells[size_t(_activeUnitCellIndex)];
}

sptrUnitCell Peak3D::unitCell(int index) const
{
    if (index < 0 || index >= int(_unitCells.size())) {
        return nullptr;
    }
    return _unitCells[size_t(index)];
}

Intensity Peak3D::getRawIntensity() const
{
    // todo: investigate whether we should scale? Probably not necessary if we use Jacobian instead of Lorentz factor
    return _rawIntensity;// * _data->getSampleStepSize();
}

Intensity Peak3D::getScaledIntensity() const
{
    return getRawIntensity() * _scale;
}

Intensity Peak3D::correctedIntensity() const
{
    auto c = _shape.center();
    auto state = _data->interpolatedState(c[2]);
    auto pos = DirectVector(_data->diffractometer()->getDetector()->pixelPosition(c[0], c[1]));
    const double lorentz = state.lorentzFactor(pos);
    const double jac = std::fabs(state.jacobianQ(DetectorEvent(c)).determinant());
    //const double factor = _scale / (lorentz * _transmission);
    const double factor = _scale * jac / _transmission;
    return getRawIntensity() * factor;
}

double Peak3D::getTransmission() const
{
    return _transmission;
}

void Peak3D::scaleShape(double scale)
{
    _shape.scale(scale);
}


double Peak3D::getScale() const
{
    return _scale;
}

void Peak3D::rescale(double factor)
{
    _scale *= factor;
}

void Peak3D::setScale(double factor)
{
    _scale = factor;

}

void Peak3D::setTransmission(double transmission)
{
    _transmission = transmission;
}

bool Peak3D::isSelected() const
{
    return (!_masked && _selected);
}

void Peak3D::setSelected(bool s)
{
    _selected = s;
}

void Peak3D::setMasked(bool masked)
{
    _masked = masked;
}

bool Peak3D::isIndexed() const
{
    return (!_unitCells.empty());
}

void Peak3D::setPredicted(bool predicted)
{
    _predicted = predicted;
}

bool Peak3D::isPredicted() const
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

double Peak3D::pValue() const
{
    return _pValue;
}

bool Peak3D::hasUnitCells() const
{
    return !_unitCells.empty();
}

int Peak3D::activeUnitCellIndex() const
{
    return _activeUnitCellIndex;
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
    auto detector = _data->diffractometer()->getDetector();
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

    const DetectorEvent event(_shape.center());    
    auto state = _data->interpolatedState(event._frame);    
    Eigen::Vector3d q0 = q().rowVector();    

    // Jacobian of map from detector coords to sample q space
    Eigen::Matrix3d J = state.jacobianQ(event);
    const Eigen::Matrix3d JI = J.inverse();

    // inverse covariance matrix in sample q space
    const Eigen::Matrix3d q_inv_cov = JI.transpose() * _shape.metric() * JI;
    return Ellipsoid(q0, q_inv_cov);
}


ReciprocalVector Peak3D::qPredicted() const
{
    auto uc = activeUnitCell();
    if (!uc) {
        return {};
    }
    auto index = MillerIndex(q(), *uc);
    return ReciprocalVector(uc->fromIndex(index.rowVector().cast<double>()));
}

DetectorEvent Peak3D::predictCenter(double frame) const
{
    const DetectorEvent no_event = {0, 0, -1, -1};
    auto uc = activeUnitCell();

    if (!uc) {
        return no_event;
    }

    auto index = MillerIndex(q(), *uc);
    auto state = _data->interpolatedState(frame);
    Eigen::RowVector3d q_hkl = uc->fromIndex(index.rowVector().cast<double>());
    Eigen::RowVector3d ki = state.ki().rowVector();
    Eigen::RowVector3d kf = q_hkl*state.sampleOrientationMatrix().transpose() + ki;

    const double alpha = ki.norm() / kf.norm();
    

    Eigen::RowVector3d kf1 = alpha*kf;
    Eigen::RowVector3d kf2 = -alpha*kf;

    Eigen::RowVector3d pred_kf = (kf1-kf).norm() < (kf2-kf).norm() ? kf1 : kf2;

    return _data->diffractometer()->getDetector()->constructEvent(DirectVector(state.samplePosition), ReciprocalVector(pred_kf*state.detectorOrientation));
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
