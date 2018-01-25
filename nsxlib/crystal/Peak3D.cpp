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
#include "MillerIndex.h"
#include "Peak3D.h"
#include "PeakIntegrator.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "Source.h"
#include "UnitCell.h"
#include "Units.h"

namespace nsx {

Peak3D::Peak3D(sptrDataSet data):
    _shape(),
    _unitCells(),
    _counts(0.0),
    _scale(1.0),
    _selected(true),
    _masked(false),
    _observed(true),
    _transmission(1.0),
    _activeUnitCellIndex(0),
    _data(data)
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


Eigen::VectorXd Peak3D::getProjection() const
{
    return _scale*_projection;
}

Eigen::VectorXd Peak3D::getPeakProjection() const
{
    return _scale*_projectionPeak;
}

Eigen::VectorXd Peak3D::getBkgProjection() const
{
    return _scale*_projectionBkg;
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
    return _intensity;// * _data->getSampleStepSize();
}

Intensity Peak3D::getScaledIntensity() const
{
    return getRawIntensity() * _scale;
}

Intensity Peak3D::getCorrectedIntensity() const
{
    auto c = _shape.center();
    auto state = _data->interpolatedState(c[2]);
    auto pos = DirectVector(_data->diffractometer()->getDetector()->pixelPosition(c[0], c[1]));
    const double factor = _scale / (state.lorentzFactor(pos) * _transmission);
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

void Peak3D::setObserved(bool observed)
{
    _observed = observed;
}

bool Peak3D::isObserved() const
{
    return _observed;
}

double Peak3D::getIOverSigmaI() const
{
    //return _counts/_countsSigma;
    return _intensity.value() / _intensity.sigma();
}

void Peak3D::updateIntegration(const PeakIntegrator& integrator)
{
    _integrationRegion = integrator.getRegion();
    _projectionPeak = integrator.getProjectionPeak();
    _projectionBkg = integrator.getProjectionBackground();
    _projection = integrator.getProjection();
    _counts = _projectionPeak.sum();
    //_countsSigma = std::sqrt(std::abs(_counts));
    _pValue = integrator.pValue();
    _intensity = integrator.getPeakIntensity();

    // fit peak profile
    //_profile.fit(_projectionPeak, 100);

    _integration = integrator;
}

double Peak3D::pValue() const
{
    return _pValue;
}

const Profile& Peak3D::getProfile() const
{
    return _profile;
}

const PeakIntegrator &Peak3D::getIntegration() const
{
    return _integration;
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
    _intensity = i; // / data()->getSampleStepSize();
}

ReciprocalVector Peak3D::getQ() const
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
//! Then if q = q0 + B(x-x0), then the corresponding ellipsoid.
//!
//! This method can throw if there is no valid q-shape corresponding to the detector space shape.
Ellipsoid Peak3D::qShape() const
{
    const Eigen::Vector3d p = _shape.center();
    const Eigen::Matrix3d A = _shape.metric();
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(A);
    const Eigen::Matrix3d U = solver.eigenvectors();
    const Eigen::Vector3d l = solver.eigenvalues();
    const Eigen::RowVector3d q = getQ().rowVector();
    auto detector = _data->diffractometer()->getDetector();
    
    Eigen::Matrix3d delta;

    for (int i = 0; i < 3; ++i) {
        const double s = 3.0 * std::sqrt(1.0 / l(i));
        Eigen::Vector3d p1 = p+s*U.col(i);
        Eigen::Vector3d p2 = p-s*U.col(i);

        auto state1 = _data->interpolatedState(p1[2]);
        auto state2 = _data->interpolatedState(p2[2]);

        const auto q1 = state1.sampleQ(DirectVector(detector->pixelPosition(p1[0], p1[1]))).rowVector();
        const auto q2 = state2.sampleQ(DirectVector(detector->pixelPosition(p2[0], p2[1]))).rowVector();
        delta.col(i) = 0.5 * (q1 - q2) / s;
    }

    // approximate linear transformation q space to detector space
    const Eigen::Matrix3d B = U * delta.inverse();
    return Ellipsoid(q.transpose(), B.transpose()*A*B);
}

} // end namespace nsx
