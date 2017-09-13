#include <cassert>
#include <cmath>
#include <stdexcept>

#include <Eigen/Dense>

#include "../crystal/Peak3D.h"
#include "../crystal/UBSolution.h"
#include "../crystal/UnitCell.h"
#include "../instrument/Component.h"
#include "../instrument/ComponentState.h"
#include "../instrument/Detector.h"
#include "../instrument/Gonio.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../instrument/TransAxis.h"
#include "../instrument/RotAxis.h"
#include "../mathematics/MatrixOperations.h"
#include "../mathematics/Minimizer.h"
#include "../utils/Units.h"

namespace nsx {

UBSolution::UBSolution(sptrSource source, sptrSample sample, sptrDetector detector, sptrUnitCell cell):
    _source(source),
    _sample(sample),
    _detector(detector),
    _cell(cell)
{
    _nSampleAxes = _sample && _sample->hasGonio() ? _sample->getGonio()->getNAxes() : 0;         
    _sampleOffset.resize(_nSampleAxes);
    _sigmaSample.resize(_nSampleAxes);
    
    _nDetectorAxes = _detector && _detector->hasGonio() ? _detector->getGonio()->getNAxes() : 0;
    _detectorOffset.resize(_nDetectorAxes);
    _sigmaDetector.resize(_nDetectorAxes);

    for (int i = 0; i < _nSampleAxes; ++i) {
        _sampleOffset[i] = _sample->getGonio()->getAxis(i)->getOffset();
        _sigmaSample[i] = 0.0;
    }
    
    for (int i = 0; i < _nDetectorAxes; ++i) {
        _detectorOffset[i] = _detector->getGonio()->getAxis(i)->getOffset();
        _sigmaDetector[i] = 0.0;
    }
    
    _sourceOffset = _source ? _source->getSelectedMonochromator().getOffset() : 0.0;
    _sigmaSource = 0.0;

    if (_cell) {
        _NP = cell->niggliTransformation();
        _uOffsets.setZero();
        _u0 = cell->busingLevyU();
    
        Eigen::Matrix3d A = cell->basis()*_NP.inverse();
        Eigen::Matrix3d G = A.transpose()*A;
    
        _initialCharacter.resize(6);
        _initialCharacter << G(0,0), G(1,1), G(2,2), G(1,2), G(0,2), G(0,1);
        _character = _initialCharacter;
    }
}

std::ostream& operator<<(std::ostream& os, const UBSolution& solution)
{
    auto& mono = solution._source->getSelectedMonochromator();

    os<<"UB matrix:"<<std::endl;
    os<<solution.ub()<< std::endl;
    os << "Wavelength:" << mono.getWavelength() << "("<< solution._sigmaSource<< ")" << std::endl;
    os<<"Detector offsets: " << std::endl;
    auto detectorG=solution._detector->getGonio();
    for (unsigned int i=0;i<detectorG->getNAxes();++i) {
        os << detectorG->getAxis(i)->getLabel() << " ";
        Axis* axis=detectorG->getAxis(i);
        if (dynamic_cast<TransAxis*>(axis) != nullptr) {
            os << solution._detectorOffset[i]/mm << "(" << solution._sigmaDetector[i]/mm << ") mm " << std::endl;
        } else if (dynamic_cast<RotAxis*>(axis) != nullptr) {
            os << solution._detectorOffset[i]/deg << "(" << solution._sigmaDetector[i]/deg << ") deg " << std::endl;
        }
    }
    os <<std::endl;
    os<<"Sample offsets:" << std::endl;
    auto sampleG=solution._sample->getGonio();
    for (unsigned int i=0;i<sampleG->getNAxes();++i) {
        os << sampleG->getAxis(i)->getLabel() << " ";
        Axis* axis=sampleG->getAxis(i);
        if (dynamic_cast<TransAxis*>(axis) != nullptr) {
            os << solution._sampleOffset[i]/mm << "(" << solution._sigmaSample[i]/mm << ") mm " << std::endl;
        } else if (dynamic_cast<RotAxis*>(axis) != nullptr) {
            os << solution._sampleOffset[i]/deg << "(" << solution._sigmaSample[i]/deg << ") deg " << std::endl;
        }
    }
    os<<std::endl;
    return os;
}

void UBSolution::apply()
{
    if (_source) {
        auto& mono = _source->getSelectedMonochromator();
        mono.setOffset(_sourceOffset);            
    }

    if (_sample) {
        if (auto sgonio = _sample->getGonio()) {
            for (unsigned int i = 0; i < sgonio->getNAxes(); ++i) {
                auto ax = sgonio->getAxis(i);
                ax->setOffset(_sampleOffset[i]);
            }
        }
    }

    if (_detector) {
        if (auto dgonio = _detector->getGonio()) {
            for (unsigned int i=0;i<dgonio->getNAxes();++i) {
                auto ax = dgonio->getAxis(i);
                ax->setOffset(_detectorOffset[i]);
            }
        }
    }

    if (_cell) {
        _cell->setReciprocalBasis(ub());
    }
}

Eigen::Matrix3d UBSolution::ub() const
{
    UnitCell uc;
    uc.setABCDEF(_character(0), _character(1), _character(2), _character(3), _character(4), _character(5));
    const double d = 1.0 / std::sqrt(1.0 + _uOffsets.squaredNorm());
    Eigen::Matrix3d du = Eigen::Quaterniond(d, d*_uOffsets(0), d*_uOffsets(1), d*_uOffsets(2)).toRotationMatrix();
    Eigen::Matrix3d A = du*_u0*uc.basis();
    return A.inverse();
}

void UBSolution::setNiggliConstraint(bool constrain, double weight)
{
    _niggliConstraint = constrain;
    _niggliWeight = weight;
}

int UBSolution::numNiggliConstraints() const
{
    if (!_niggliConstraint || !_cell) {
        return 0;
    }
    return _cell->niggliCharacter().C.rows();
}

Eigen::VectorXd UBSolution::niggliConstraints() const
{
    if (numNiggliConstraints() <= 0) {
        return {};
    }
    const Eigen::Matrix3d NA = ub().inverse();
    const Eigen::Matrix3d G = NA.transpose()*NA;
    const Eigen::MatrixXd C = _cell->niggliCharacter().C;
    Eigen::VectorXd x(6);
    x << G(0,0), G(1,1), G(2,2), G(1,2), G(0,2), G(0,1);
    return C * x;
}

double UBSolution::niggliWeight() const
{
    return _niggliWeight;
}

} // end namespace nsx
