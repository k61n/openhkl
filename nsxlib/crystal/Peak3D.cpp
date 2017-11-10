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
#include "DetectorEvent.h"
#include "Diffractometer.h"
#include "GeometryTypes.h"
#include "Gonio.h"
#include "IFrameIterator.h"
#include "InstrumentState.h"
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

#if 0
Peak3D::Peak3D(const Peak3D& other): Peak3D(other.data)
{
    *this = other;
}
#endif

#if 0
Peak3D& Peak3D::operator=(const Peak3D& other)
{
    // nothing to do
    if (this == &other) {
        return *this;
    }

    _shape = other._shape;
    _projection = other._projection;
    _projectionPeak = other._projectionPeak;
    _projectionBkg = other._projectionBkg;
    _unitCells = other._unitCells;       
    _counts = other._counts;
    _scale = other._scale;
    _selected = other._selected;
    _observed = other._observed;
    _masked = other._masked;
    _transmission = other._transmission;
    _activeUnitCellIndex = other._activeUnitCellIndex;

    return *this;
}
#endif
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

sptrUnitCell Peak3D::getActiveUnitCell() const
{
    if (_activeUnitCellIndex < 0 || _activeUnitCellIndex >= int(_unitCells.size())) {
        return nullptr;
    }
    return _unitCells[size_t(_activeUnitCellIndex)];
}

sptrUnitCell Peak3D::getUnitCell(int index) const
{
    if (index < 0 || index >= int(_unitCells.size())) {
        return nullptr;
    }
    return _unitCells[size_t(index)];
}

Intensity Peak3D::getRawIntensity() const
{
    return _intensity * _data->getSampleStepSize();
}

Intensity Peak3D::getScaledIntensity() const
{
    return getRawIntensity() * _scale;
}

Intensity Peak3D::getCorrectedIntensity() const
{
    auto q = getQ();
    auto c = _shape.center();
    auto ev = DetectorEvent(_data, c[0], c[1], c[2]);
    const double factor = _scale / (ev.getLorentzFactor() * _transmission);
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

#if 0
bool operator<(const Peak3D& p1, const Peak3D& p2)
{
    Eigen::RowVector3d hkl1, hkl2;

    p1.getMillerIndices(hkl1, true);
    p2.getMillerIndices(hkl2, true);

    if (hkl1[0]<hkl2[0]) {
        return true;
    }
    if (hkl1[0]>hkl2[0]) {
        return false;
    }
    if (hkl1[1]<hkl2[1]) {
        return true;
    }
    if (hkl1[1]>hkl2[1]) {
        return false;
    }
    if (hkl1[2]<hkl2[2]) {
        return true;
    }
    if (hkl1[2]>hkl2[2]) {
        return false;
    }
    return false;
}
#endif

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

bool Peak3D::isMasked() const
{
    return _masked;
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

int Peak3D::getActiveUnitCellIndex() const
{
    return _activeUnitCellIndex;
}

ReciprocalVector Peak3D::getQ() const
{
    auto p = _shape.center();
    auto kf = DetectorEvent(_data, p[0], p[1], p[2]).Kf();
    auto state = _data->getInterpolatedState(p[2]);
    auto q = static_cast<const Eigen::RowVector3d&>(kf);
    q[1] -= 1.0/_data->getDiffractometer()->getSource()->getSelectedMonochromator().getWavelength();
    return ReciprocalVector(state.sample.transformQ(q));
}

void Peak3D::setRawIntensity(const Intensity& i)
{  
    // note: the scaling factor is taken to be consistent with Peak3D::getRawIntensity()
    _intensity = i / data()->getSampleStepSize();
}

} // end namespace nsx
