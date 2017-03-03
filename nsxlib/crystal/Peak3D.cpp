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

#include "../instrument/ComponentState.h"
#include "../instrument/InstrumentState.h"
#include "../instrument/DetectorEvent.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Gonio.h"
#include "../data/IData.h"
#include "../geometry/IShape.h"
#include "Peak3D.h"
#include "PeakIntegrator.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../utils/Units.h"
#include "../geometry/Blob3D.h"
#include "../data/IData.h"
#include "../utils/Types.h"

#include "../geometry/Ellipsoid.h"
#include "../data/IFrameIterator.h"

using SX::Geometry::Blob3D;

using SX::Data::IFrameIterator;
using SX::Instrument::DetectorEvent;
using SX::Instrument::ComponentState;

namespace SX {
namespace Crystal {

using shape_type = Peak3D::shape_type;

Peak3D::Peak3D(std::shared_ptr<SX::Data::DataSet> data):
    _data(),
    _shape(),
    _unitCells(),
    _sampleState(nullptr),
    _event(nullptr),
    _source(nullptr),
    _counts(0.0),
    _countsSigma(0.0),
    _scale(1.0),
    _selected(true),
    _masked(false),
    _observed(true),
    _transmission(1.0),
    _activeUnitCellIndex(0)
{
    linkData(data);
}

Peak3D::Peak3D(std::shared_ptr<Data::DataSet> data, const Peak3D::Ellipsoid3D &shape):
    Peak3D(data)
{
    setShape(shape);
}

Peak3D::Peak3D(const Peak3D& other):
    _data(other._data),
    _shape(other._shape),
    _projection(other._projection),
    _projectionPeak(other._projectionPeak),
    _projectionBkg(other._projectionBkg),
    _unitCells(other._unitCells),
    _sampleState(other._sampleState == nullptr ? nullptr : new ComponentState(*other._sampleState)),
    _event(other._event == nullptr ? nullptr : new DetectorEvent(*other._event)),
    _source(other._source),
    _counts(other._counts),
    _countsSigma(other._countsSigma),
    _scale(other._scale),
    _selected(other._selected),
    _masked(other._masked),
    _observed(other._observed),
    _transmission(other._transmission),
    _activeUnitCellIndex(other._activeUnitCellIndex)
{
}

Peak3D& Peak3D::operator=(const Peak3D& other)
{
    // nothing to do
    if (this == &other) {
        return *this;
    }

    _data = other._data;
    _shape = other._shape;
    _projection = other._projection;
    _projectionPeak = other._projectionPeak;
    _projectionBkg = other._projectionBkg;
    _unitCells = other._unitCells;
    _sampleState = other._sampleState == nullptr ? nullptr : std::unique_ptr<ComponentState>(new ComponentState(*other._sampleState));
    _event = other._event == nullptr ? nullptr : std::unique_ptr<DetectorEvent>(new DetectorEvent(*other._event));
    _source= other._source;
    _counts = other._counts;
    _countsSigma = other._countsSigma;
    _scale = other._scale;
    _selected = other._selected;
    _observed = other._observed;
    _masked = other._masked;
    _transmission = other._transmission;
    _activeUnitCellIndex = other._activeUnitCellIndex;

    return *this;
}

void Peak3D::linkData(const std::shared_ptr<SX::Data::DataSet>& data)
{
    _data = std::weak_ptr<SX::Data::DataSet>(data);
    if (data != nullptr) {
        setSource(data->getDiffractometer()->getSource());
    }
}

void Peak3D::unlinkData()
{
    _data.reset();
}

Eigen::RowVector3d Peak3D::getMillerIndices() const
{
    Eigen::RowVector3d hkld;
    getMillerIndices(hkld, true);
    return hkld;
}

void Peak3D::setShape(const Ellipsoid3D& peak)
{
    using DetectorEvent = SX::Instrument::DetectorEvent;
    _shape = peak;

    Eigen::Vector3d center = peak.getAABBCenter();
    //int f = int(std::lround(std::floor(center[2])));
    double f = std::min(center[2], double(getData()->getNFrames())-1.0001);

    auto data = getData();

    const auto& state = data->getInterpolatedState(f);

    setSampleState(ComponentState(state.sample));

    setDetectorEvent(DetectorEvent(
       *data->getDiffractometer()->getDetector(), center[0], center[1], state.detector.getValues()));
}

bool Peak3D::getMillerIndices(const UnitCell& uc, Eigen::RowVector3d& hkl, bool applyUCTolerance) const
{
    hkl = uc.fromReciprocalStandard(getQ());

    if (applyUCTolerance) {
        double tolerance = uc.getHKLTolerance();

        if (std::fabs(hkl[0]-std::round(hkl[0])) < tolerance &&
                std::fabs(hkl[1]-std::round(hkl[1])) < tolerance &&
                std::fabs(hkl[2]-std::round(hkl[2])) < tolerance) {
            hkl[0]=std::round(hkl[0]);
            hkl[1]=std::round(hkl[1]);
            hkl[2]=std::round(hkl[2]);
            return true;
        }
        hkl = Eigen::Vector3d::Zero();
        return false;
    }
    return true;
}

bool Peak3D::getMillerIndices(int ucIndex, Eigen::RowVector3d& hkl, bool applyUCTolerance) const
{
    if (_unitCells.empty() || ucIndex < 0 || ucIndex >= int(_unitCells.size())) {
        hkl = Eigen::Vector3d::Zero();
        return false;
    }
    sptrUnitCell uc = _unitCells[size_t(ucIndex)];
    return getMillerIndices(*uc, hkl, applyUCTolerance);
}

bool Peak3D::getMillerIndices(Eigen::RowVector3d& hkl, bool applyUCTolerance) const
{
    return getMillerIndices(_activeUnitCellIndex, hkl, applyUCTolerance);
}

Eigen::RowVector3i Peak3D::getIntegerMillerIndices() const
{
    Eigen::RowVector3d hkld;
    getMillerIndices(hkld, true);
    Eigen::RowVector3i hkl;
    hkl << int(std::lround(hkld[0])), int(std::lround(hkld[1])), int(std::lround(hkld[2]));
    return hkl;
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

Eigen::VectorXd Peak3D::getProjectionSigma() const
{
    return _scale*(_projection.array().sqrt());
}

Eigen::VectorXd Peak3D::getPeakProjectionSigma() const
{
    return _scale*(_projectionPeak.array().sqrt());
}

Eigen::VectorXd Peak3D::getBkgProjectionSigma() const
{
    return _scale*(_projectionBkg.array().sqrt());
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

double Peak3D::getRawIntensity() const
{
    return _counts * getData()->getSampleStepSize();
}

double Peak3D::getScaledIntensity() const
{
    return _scale*getRawIntensity();
}

double Peak3D::getTransmission() const
{
    return _transmission;
}

void Peak3D::scaleShape(double scale)
{
    _shape.scale(scale);
}

double Peak3D::getRawSigma() const
{
    return _countsSigma * getData()->getSampleStepSize();
}

double Peak3D::getScaledSigma() const
{
    return _scale*getRawSigma();
}

double Peak3D::getLorentzFactor() const
{
    double gamma,nu;
    this->getGammaNu(gamma,nu);
    double lorentz = 1.0/(sin(std::fabs(gamma))*cos(nu));
    return lorentz;
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

const ComponentState& Peak3D::getSampleState()
{
    assert(_sampleState != nullptr);
    return *_sampleState;
}

Eigen::RowVector3d Peak3D::getKf() const
{
    assert(_source != nullptr);
    assert(_sampleState != nullptr);
    double wavelength = _source->getSelectedMonochromator().getWavelength();
    Eigen::Vector3d kf = _event->getKf(wavelength, _sampleState->getPosition());
    return kf;
}

Eigen::RowVector3d Peak3D::getQ() const
{
    assert(_source != nullptr);

    double wavelength = _source->getSelectedMonochromator().getWavelength();

    // If sample state is not set, assume sample is at the origin
    if (!_sampleState) {
        return _event->getQ(wavelength);
    }

    // otherwise scattering point is deducted from the sample
    Eigen::Vector3d q = _event->getQ(wavelength, _sampleState->getPosition());
    return _sampleState->transformQ(q);
}

void Peak3D::setSampleState(const ComponentState& sstate)
{
    _sampleState = std::unique_ptr<ComponentState>(new ComponentState(sstate));
}

void Peak3D::setDetectorEvent(const SX::Instrument::DetectorEvent& event)
{
    _event = std::unique_ptr<DetectorEvent>(new DetectorEvent(event));
}

void Peak3D::setSource(const std::shared_ptr<SX::Instrument::Source>& source)
{
    _source = source;
}

void Peak3D::getGammaNu(double& gamma,double& nu) const
{
    assert(_sampleState != nullptr);
    _event->getGammaNu(gamma,nu,_sampleState->getPosition());
}

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
    return _counts/_countsSigma;
}

void Peak3D::updateIntegration(const PeakIntegrator& integrator)
{
    _integrationRegion = integrator.getRegion();
    _projectionPeak = integrator.getProjectionPeak();
    _projectionBkg = integrator.getProjectionBackground();
    _projection = integrator.getProjection();
    _counts = _projectionPeak.sum();
    _countsSigma = std::sqrt(std::abs(_counts));
}

double Peak3D::pValue()
{
    // assumption: all background pixel counts are Poisson processes with rate R
    // therefore, we can estimate the rate R as below:
    const double R = _countsBkg.sum() / _pointsBkg.sum();

    // null hypothesis: the pixels inside the peak are Poisson processes with rate R
    // therefore, by central limit theorem the average value
    const double avg = _countsPeak.sum() / _pointsPeak.sum();
    // is normal with mean R and variance:
    const double var = R / _pointsPeak.sum();

    // thus we obtain the following standard normal variable
    const double z = (avg-R) / std::sqrt(var);

    // compute the p value
    const double p = 1.0 - 0.5 * (1.0 + std::erf(z / std::sqrt(2)));

    return p;
}

bool Peak3D::hasUnitCells() const
{
    return !_unitCells.empty();
}

int Peak3D::getActiveUnitCellIndex() const
{
    return _activeUnitCellIndex;
}

} // namespace Crystal
} // namespace SX
