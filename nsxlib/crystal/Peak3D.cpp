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

#include "ComponentState.h"
#include "DetectorEvent.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Gonio.h"
#include "IData.h"
#include "IShape.h"
#include "Peak3D.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"
#include "Blob3D.h"
#include "IData.h"
#include "Types.h"

#include "Ellipsoid.h"
#include "IFrameIterator.h"

using SX::Geometry::Blob3D;

using SX::Data::IFrameIterator;

namespace SX {
namespace Crystal {

using shape_type = Peak3D::shape_type;

Peak3D::Peak3D(std::shared_ptr<SX::Data::IData> data):
    _data(),
//		_hkl(Eigen::Vector3d::Zero()),
        _peak(nullptr),
        _bkg(nullptr),
        _unitCells(),
        _sampleState(nullptr),
        _event(nullptr),
        _source(nullptr),
        _counts(0.0),
        _countsSigma(0.0),
        _scale(1.0),
        _selected(true),
        _observed(true),
        _masked(false),
        _transmission(1.0),
        _state(),
        _activeUnitCellIndex(0)
{
    linkData(data);
}

Peak3D::Peak3D(std::shared_ptr<Data::IData> data, const Blob3D &blob, double confidence)
: Peak3D(data)
{
    Eigen::Vector3d center, eigenvalues;
    Eigen::Matrix3d eigenvectors;

    blob.toEllipsoid(confidence, center, eigenvalues, eigenvectors);
    setPeakShape(new Ellipsoid3D(center,eigenvalues,eigenvectors));

    eigenvalues[0]*=2.0;
    eigenvalues[1]*=2.0;
    eigenvalues[2]*=3.0;

    setBackgroundShape(new Ellipsoid3D(center,eigenvalues,eigenvectors));
}

Peak3D::Peak3D(const Peak3D& other):
        _data(other._data),
//		_hkl(other._hkl),
        _peak(other._peak == nullptr ? nullptr : other._peak->clone()),
        _bkg(other._bkg == nullptr ? nullptr : other._bkg->clone()),
        _projection(other._projection),
        _projectionPeak(other._projectionPeak),
        _projectionBkg(other._projectionBkg),
        _unitCells(other._unitCells),
        _sampleState(other._sampleState),
        _event(other._event),
        _source(other._source),
        _counts(other._counts),
        _countsSigma(other._countsSigma),
        _scale(other._scale),
        _selected(other._selected),
        _observed(other._observed),
        _masked(other._masked),
        _transmission(other._transmission),
        _state(other._state),
        _activeUnitCellIndex(other._activeUnitCellIndex)
{
}

Peak3D& Peak3D::operator=(const Peak3D& other)
{
    if (this != &other) {

        _data = other._data;
        _hkl = other._hkl;

        _peak = other._peak ? std::unique_ptr<shape_type>(other._peak->clone()) : nullptr;
        _bkg = other._bkg ? std::unique_ptr<shape_type>(other._bkg->clone()) : nullptr;

        _projection = other._projection;
        _projectionPeak = other._projectionPeak;
        _projectionBkg = other._projectionBkg;

        _unitCells = other._unitCells;
        _sampleState = other._sampleState;
        _event = other._event;
        _source= other._source;
        _counts = other._counts;
        _countsSigma = other._countsSigma;
        _scale = other._scale;
        _selected = other._selected;
        _observed = other._observed;
        _masked = other._masked;
        _transmission = other._transmission;

        _state = other._state;

        _activeUnitCellIndex = other._activeUnitCellIndex;

    }
    return *this;
}

void Peak3D::linkData(const std::shared_ptr<SX::Data::IData>& data)
{
    _data = std::weak_ptr<SX::Data::IData>(data);
    if (data != nullptr) {
        setSource(data->getDiffractometer()->getSource());
    }
}

void Peak3D::unlinkData()
{
    _data.reset();
}

void Peak3D::setPeakShape(shape_type* p)
{
    _peak = std::unique_ptr<SX::Geometry::IShape<double,3>>(p);
    Eigen::Vector3d center = _peak->getAABBCenter();
    int f = int(std::lround(std::floor(center[2])));

    using ComponentState = SX::Instrument::ComponentState;

    auto data = getData();

    setSampleState(std::make_shared<ComponentState>(data->getSampleInterpolatedState(f)));
    ComponentState detState = data->getDetectorInterpolatedState(f);

    using DetectorEvent = SX::Instrument::DetectorEvent;

    setDetectorEvent(std::make_shared<DetectorEvent>(
        data->getDiffractometer()->getDetector()->createDetectorEvent(center[0],center[1],detState.getValues())));
}

// std::shared_ptr<SX::Geometry::IShape<double,3>> Peak3D::getPeak()
// {
// 	return _peak;
// }

// std::shared_ptr<SX::Geometry::IShape<double,3>> Peak3D::getBackground()
// {
// 	return _bkg;
// }

void Peak3D::setBackgroundShape(shape_type* b)
{
    _bkg = std::unique_ptr<shape_type>(b);
}


//void Peak3D::setMillerIndices(double h, double k, double l)
//{
//	_hkl << h,k,l;
//}

//const Eigen::RowVector3d& Peak3D::getMillerIndices() const
//{
//	return _hkl;
//}

bool Peak3D::getMillerIndices(UnitCell uc, Eigen::RowVector3d& hkl, bool applyUCTolerance) const
{
    hkl = uc.fromReciprocalStandard(getQ());

    if (applyUCTolerance)
    {
        double tolerance = uc.getHKLTolerance();

        if (std::fabs(hkl[0]-std::round(hkl[0])) < tolerance &&
            std::fabs(hkl[1]-std::round(hkl[1])) < tolerance &&
            std::fabs(hkl[2]-std::round(hkl[2])) < tolerance)
        {
            hkl[0]=std::round(hkl[0]);
            hkl[1]=std::round(hkl[1]);
            hkl[2]=std::round(hkl[2]);
            return true;
        }
        else
        {
            hkl = Eigen::Vector3d::Zero();
            return false;
        }
    }
    else
        return true;
}

bool Peak3D::getMillerIndices(int ucIndex, Eigen::RowVector3d& hkl, bool applyUCTolerance) const
{
    if (_unitCells.empty() || ucIndex < 0 || ucIndex >= _unitCells.size())
    {
        hkl = Eigen::Vector3d::Zero();
        return false;
    }

    sptrUnitCell uc = _unitCells[ucIndex];

    return getMillerIndices(*uc, hkl, applyUCTolerance);
}

bool Peak3D::getMillerIndices(Eigen::RowVector3d& hkl, bool applyUCTolerance) const
{
    return getMillerIndices(_activeUnitCellIndex, hkl, applyUCTolerance);
}

Eigen::RowVector3i Peak3D::getIntegerMillerIndices() const
{
    Eigen::RowVector3d hkld;
    bool success = getMillerIndices(hkld,true);
    Eigen::RowVector3i hkl;
    hkl << std::lround(hkld[0]), std::lround(hkld[1]), std::lround(hkld[2]);
    return hkl;
}

void Peak3D::integrate()
{
    auto data = getData();

    if (data == nullptr) {
        return;
    }

    framewiseIntegrateBegin();

    unsigned int idx = _state.data_start;
    std::unique_ptr<IFrameIterator> it = data->getIterator(int(idx));

    for(; idx <= _state.data_end; it->advance(), ++idx) {
        Eigen::MatrixXi frame = it->getFrame().cast<int>();
        framewiseIntegrateStep(frame, idx);
    }

    framewiseIntegrateEnd();
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

    if (it != _unitCells.end())
        return;

    _unitCells.push_back(uc);

    if (activate)
        _activeUnitCellIndex = _unitCells.size()-1;
}

sptrUnitCell Peak3D::getActiveUnitCell() const
{
    if (_activeUnitCellIndex < 0 || _activeUnitCellIndex >= _unitCells.size())
        return nullptr;

    return _unitCells[_activeUnitCellIndex];
}

sptrUnitCell Peak3D::getUnitCell(int index) const
{
    if (index < 0 || index >= _unitCells.size())
        return nullptr;

    return _unitCells[index];
}

//bool Peak3D::hasIntegerHKL(const SX::Crystal::UnitCell& basis)
//{
//	double tolerance = basis.getHKLTolerance();
//
//	_hkl=basis.fromReciprocalStandard(this->getQ());
//	if (std::fabs(_hkl[0]-std::round(_hkl[0])) < tolerance &&
//		std::fabs(_hkl[1]-std::round(_hkl[1])) < tolerance &&
//		std::fabs(_hkl[2]-std::round(_hkl[2])) < tolerance)
//	{
//		_hkl[0]=std::round(_hkl[0]);
//		_hkl[1]=std::round(_hkl[1]);
//		_hkl[2]=std::round(_hkl[2]);
//
//		return true;
//	}
//
//	return false;
//}


double Peak3D::getRawIntensity() const
{
    return _counts * getSampleStepSize();
}

double Peak3D::getScaledIntensity() const
{
    return _scale*getRawIntensity();
}

double Peak3D::getTransmission() const
{
    return _transmission;
}

void Peak3D::scalePeakShape(double scale)
{
    _peak->scale(scale);
}

void Peak3D::scaleBackgroundShape(double scale)
{
    _bkg->scale(scale);
}

double Peak3D::getRawSigma() const
{
    return _countsSigma * getSampleStepSize();
}

double Peak3D::getScaledSigma() const
{
    return _scale*getRawSigma();
}

double Peak3D::getLorentzFactor() const
{
    double gamma,nu;
    this->getGammaNu(gamma,nu);
    return 1.0/(sin(std::fabs(gamma))*cos(nu));
}

double Peak3D::getScale() const
{
    return _scale;
}

void Peak3D::rescale(double scale)
{
    _scale *= scale;
}

void Peak3D::setScale(double scale)
{
    _scale = scale;

}

void Peak3D::setTransmission(double transmission)
{
    _transmission=transmission;
}

std::shared_ptr<SX::Instrument::DetectorEvent> Peak3D::getDetectorEvent()
{
    return _event;
}

std::shared_ptr<SX::Instrument::ComponentState> Peak3D::getSampleState()
{
    return _sampleState;
}

double Peak3D::getSampleStepSize() const
{
    // TODO(jonathan): we should NOT assume that gonio axis 0 is the one being rotated
    // when we compute 'step' below
    double step = 0.0;

    auto data = getData();

    size_t numFrames = data->getNFrames();
    const auto& ss = data->getSampleStates();
    size_t numValues = ss[0].getValues().size();

    for (size_t i = 0; i < numValues; ++i) {
        double dx = ss[numFrames-1].getValues()[i] - ss[0].getValues()[i];
        step += dx*dx;
    }

    step = std::sqrt(step);
    step /= (numFrames-1) * 0.05 * SX::Units::deg;

    return step;
}

Eigen::RowVector3d Peak3D::getKf() const
{
    double wav = _source->getWavelength();
    Eigen::Vector3d kf = _event->getParent()->getKf(*_event, wav, _sampleState->getParent()->getPosition(*_sampleState));
    return kf;
}

Eigen::RowVector3d Peak3D::getQ() const
{
    double wav=_source->getWavelength();
    // If sample state is not set, assume sample is at the origin
    if (!_sampleState) {
        return _event->getParent()->getQ(*_event, wav);
    }

    // otherwise scattering point is deducted from the sample
    Eigen::Vector3d q = _event->getParent()->getQ(*_event, wav,
                                                  _sampleState->getParent()->getPosition(*_sampleState));
    q = _sampleState->getParent()->getGonio()->getInverseHomMatrix(_sampleState->getValues()).rotation()*q;
    return q;
}

void Peak3D::setSampleState(const std::shared_ptr<SX::Instrument::ComponentState>& sstate)
{
    _sampleState = sstate;
}

void Peak3D::setDetectorEvent(const std::shared_ptr<SX::Instrument::DetectorEvent>& event)
{
    _event = event;
}

void Peak3D::setSource(const std::shared_ptr<SX::Instrument::Source>& source)
{
    _source = source;
}

void Peak3D::getGammaNu(double& gamma,double& nu) const
{
    _event->getParent()->getGammaNu(*_event,gamma,nu,_sampleState->getParent()->getPosition(*_sampleState));
}

bool operator<(const Peak3D& p1, const Peak3D& p2)
{
    Eigen::RowVector3d hkl1,hkl2;

    bool success;
    success = p1.getMillerIndices(hkl1,true);
    success = p2.getMillerIndices(hkl2,true);

    if (hkl1[0]<hkl2[0])
        return true;
    else if (hkl1[0]>hkl2[0])
        return false;
    if (hkl1[1]<hkl2[1])
        return true;
    else if (hkl1[1]>hkl2[1])
        return false;
    if (hkl1[2]<hkl2[2])
        return true;
    else if (hkl1[2]>hkl2[2])
        return false;
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

void Peak3D::framewiseIntegrateBegin()
{
    auto data = getData();

    if (!data) {
        return;
    }

    // Get the lower and upper limit of the bkg Bounding box
    _state.lower = _bkg->getLower();
    _state.upper= _bkg->getUpper();

    //
    _state.data_start = static_cast<unsigned int>(std::floor(_state.lower[2]));
    _state.data_end = static_cast<unsigned int>(std::ceil(_state.upper[2]));

    _state.start_x = static_cast<unsigned int>(std::floor(_state.lower[0]));
    _state.end_x = static_cast<unsigned int>(std::ceil(_state.upper[0]));

    _state.start_y = static_cast<unsigned int>(std::floor(_state.lower[1]));
    _state.end_y = static_cast<unsigned int>(std::ceil(_state.upper[1]));

    if (_state.lower[0] < 0) {
        _state.start_x=0;
    }
    if (_state.lower[1] < 0) {
        _state.start_y=0;
    }
    if (_state.lower[2] < 0) {
        _state.data_start=0;
    }

    if (_state.end_x > data->getNCols()-1) {
        _state.end_x = static_cast<unsigned int>(data->getNCols()-1);
    }
    if (_state.end_y > data->getNRows()-1) {
        _state.end_y =  static_cast<unsigned int>(data->getNRows()-1);
    }
    if (_state.data_end > data->getNFrames()-1) {
        _state.data_end = static_cast<unsigned int>(data->getNFrames()-1);
    }

    // Allocate all vectors
    _projection = Eigen::VectorXd::Zero(_state.data_end - _state.data_start + 1);
    _projectionPeak = Eigen::VectorXd::Zero(_state.data_end - _state.data_start + 1);
    _projectionBkg = Eigen::VectorXd::Zero(_state.data_end - _state.data_start + 1);
    _pointsPeak = Eigen::VectorXd::Zero(_state.data_end - _state.data_start + 1);
    _pointsBkg = Eigen::VectorXd::Zero(_state.data_end - _state.data_start + 1);
    _countsPeak = Eigen::VectorXd::Zero(_state.data_end - _state.data_start + 1);
    _countsBkg = Eigen::VectorXd::Zero(_state.data_end - _state.data_start + 1);

    _state.dx = int(_state.end_x - _state.start_x);
    _state.dy = int(_state.end_y - _state.start_y);

    return;
}


void Peak3D::framewiseIntegrateStep(Eigen::MatrixXi& frame, unsigned int idx)
{
    auto data = getData();

    if (!data || idx < _state.data_start || idx > _state.data_end) {
        return;
    }

    double pointsinpeak = 0;
    double pointsinbkg = 0;
    double intensityP = 0;
    double intensityBkg = 0;

    _projection[idx-_state.data_start] += frame.block(_state.start_y, _state.start_x, _state.dy,_state.dx).sum();

    for (unsigned int x = _state.start_x; x <= _state.end_x; ++x) {
        for (unsigned int y = _state.start_y; y <= _state.end_y; ++y) {
            int intensity = frame(y, x);
            _state.point1 << x+0.5, y+0.5, idx, 1;

            bool inbackground = (_bkg->isInsideAABB(_state.point1) && _bkg->isInside(_state.point1));
            bool inpeak = (_peak->isInsideAABB(_state.point1) && _peak->isInside(_state.point1));

            if (inpeak) {
                intensityP += intensity;
                pointsinpeak++;
                continue;
            }
            else if (inbackground) {
                intensityBkg += intensity;
                pointsinbkg++;
            }
        }
    }

    _pointsPeak[idx-_state.data_start] = pointsinpeak;
    _pointsBkg[idx-_state.data_start] = pointsinbkg;

    _countsPeak[idx-_state.data_start] = intensityP;
    _countsBkg[idx-_state.data_start] = intensityBkg;

    if (pointsinpeak > 0) {
        _projectionPeak[idx-_state.data_start] = intensityP-intensityBkg*pointsinpeak/pointsinbkg;
    }

    return;
}


void Peak3D::framewiseIntegrateEnd()
{
    auto data = getData();

    if (!data)
        return;

    // Quick fix determine the limits of the peak range
    int datastart = 0;
    int dataend = 0;
    bool startfound = false;

    for (int i = 0; i < _projectionPeak.size(); ++i) {
        if (!startfound && std::fabs(_projectionPeak[i]) > 1e-6) {
            datastart = i;
            startfound = true;
        }
        if (startfound) {
            if (std::fabs(_projectionPeak[i])<1e-6) {
                dataend = i;
                break;
            }
        }
    }
    //

    Eigen::VectorXd bkg=_projection-_projectionPeak;
    if (datastart>1)
        datastart--;

    // Safety check
    if (datastart==dataend)
        return;

    double bkg_left=bkg[datastart];
    double bkg_right=bkg[dataend];
    double diff;
    for (int i=datastart;i<dataend;++i) {
        diff=bkg[i]-(bkg_left+static_cast<double>((i-datastart))/static_cast<double>((dataend-datastart))*(bkg_right-bkg_left));
        if (diff>0)
            _projectionPeak[i]+=diff;
    }

    // note: this "background" simply refers to anything in the AABB but NOT in the peak
    _projectionBkg=_projection-_projectionPeak;
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

} // Crsystal
} // SX
