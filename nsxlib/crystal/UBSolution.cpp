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

int UBSolution::inputs() const
{
    // 9 UB parameters + wavelength
    int nInputs=10;

    if (_detector && _detector->hasGonio()) {
        auto gonio = _detector->getGonio();
        nInputs += gonio->getNAxes();
    }
    if (_sample && _sample->hasGonio()) {
        auto gonio = _sample->getGonio();
        nInputs += gonio->getNAxes();
    }
    return nInputs;
}


void UBSolution::resetParameters()
{
    _sourceOffset = 0.0;
    _sigmaSourceOffset = 0.0;

    if (_nSampleAxes > 0)  {
        _sampleOffsets *= 0;
        _sigmaSampleOffsets *= 0;
    }

    if (_nDetectorAxes > 0) {
        _detectorOffsets *= 0;
        _sigmaDetectorOffsets *= 0;
    }

    if (_cell) {
        _uOffsets.setZero();
        _character = _character0;
        _covub.setZero();
    }
}

void UBSolution::refineSample(unsigned int id, bool refine)
{
    if (id >= _nSampleAxes) {
        throw std::runtime_error("id(" + std::to_string(id) + ") is out of range (" + std::to_string(_nSampleAxes) + ")");
    }
    _refineSample[id] = refine;
}

void UBSolution::refineSource(bool refine)
{
    _refineSource = refine;
}

void UBSolution::refineDetector(unsigned int id, bool refine)
{
    if (id >= _nDetectorAxes) {
        throw std::runtime_error("id(" + std::to_string(id) + ") is out of range (" + std::to_string(_nDetectorAxes) + ")");
    }
    _refineDetector[id] = refine;
}

Eigen::VectorXd UBSolution::zip() const
{
    const int n = 9 + 1 + _nSampleAxes + _nDetectorAxes;
    Eigen::VectorXd x(n);

    x(0) = _uOffsets(0);
    x(1) = _uOffsets(1);
    x(2) = _uOffsets(2);

    Eigen::VectorXd y;

    if (numNiggliConstraints() == 0) {
        y = _character;
    } else {
        Eigen::MatrixXd kernel = Eigen::FullPivLU<Eigen::MatrixXd>(_cell->niggliCharacter().C).kernel();
        // get starting values
        y = (kernel.transpose()*kernel).inverse()*kernel.transpose()*_character;
    }

    for (auto i = 0; i < 6; ++i) {
        x(3+i) = i < y.size() ? y(i) : 0.0;
    }

    x(9) = _sourceOffset;

    if (_nSampleAxes > 0) {
        x.segment(10,_nSampleAxes) = _sampleOffsets;
    }

    if (_nDetectorAxes > 0) {
        x.segment(10+_nSampleAxes, _nDetectorAxes) = _detectorOffsets;
    }

    return x;
}

void UBSolution::unzip(const Eigen::VectorXd& x) 
{
    assert(x.size() == inputs());
    Eigen::VectorXd y(6);

    _uOffsets << x(0), x(1), x(2);
    y << x(3), x(4), x(5), x(6), x(7), x(8);

    // apply constraints
    if (numNiggliConstraints() > 0) {
        _character.resize(6);
        _character.setZero();
        Eigen::MatrixXd kernel = Eigen::FullPivLU<Eigen::MatrixXd>(_cell->niggliCharacter().C).kernel();
        std::cout << "debugging:\n" << kernel << std::endl;
        for (auto i = 0; i < kernel.cols(); ++i) {
            _character += y(i)*kernel.col(i);
        }
    } else {
        _character = y;
    }

    _sourceOffset = x(9);

    if (_nSampleAxes > 0) {
        _sampleOffsets = x.segment(10,_nSampleAxes);
    }

    if (_nDetectorAxes > 0) {
        _detectorOffsets = x.segment(10+_nSampleAxes, _nDetectorAxes);
    }
}

void UBSolution::setValue(unsigned int idx, double value)
{
    assert(idx >= 0);
    assert(idx < inputs());
    Eigen::VectorXd x = zip();
    x(idx) = value;
    unzip(x);
}

std::ostream& operator<<(std::ostream& os, const UBSolution& solution)
{
    auto& mono = solution._source->getSelectedMonochromator();

    os<<"UB matrix:"<<std::endl;
    os<<solution.ub()<< std::endl;
    os << "Wavelength:" << mono.getWavelength() << "("<< solution._sigmaSourceOffset<< ")" << std::endl;
    os<<"Detector offsets: " << std::endl;
    auto detectorG=solution._detector->getGonio();
    for (unsigned int i=0;i<detectorG->getNAxes();++i) {
        os << detectorG->getAxis(i)->getLabel() << " ";
        Axis* axis=detectorG->getAxis(i);
        if (dynamic_cast<TransAxis*>(axis) != nullptr) {
            os << solution._detectorOffsets[i]/mm << "(" << solution._sigmaDetectorOffsets[i]/mm << ") mm " << std::endl;
        } else if (dynamic_cast<RotAxis*>(axis) != nullptr) {
            os << solution._detectorOffsets[i]/deg << "(" << solution._sigmaDetectorOffsets[i]/deg << ") deg " << std::endl;
        }
    }
    os <<std::endl;
    os<<"Sample offsets:" << std::endl;
    auto sampleG=solution._sample->getGonio();
    for (unsigned int i=0;i<sampleG->getNAxes();++i) {
        os << sampleG->getAxis(i)->getLabel() << " ";
        Axis* axis=sampleG->getAxis(i);
        if (dynamic_cast<TransAxis*>(axis) != nullptr) {
            os << solution._sampleOffsets[i]/mm << "(" << solution._sigmaSampleOffsets[i]/mm << ") mm " << std::endl;
        } else if (dynamic_cast<RotAxis*>(axis) != nullptr) {
            os << solution._sampleOffsets[i]/deg << "(" << solution._sigmaSampleOffsets[i]/deg << ") deg " << std::endl;
        }
    }
    os<<std::endl;
    return os;
}

void UBSolution::apply()
{
    if (!_detector || !_sample || !_source) {
        throw std::runtime_error("A detector, sample and source must be specified prior to calculate residuals.");
    }
    // Parameter 9 is offset in wavelength
    if (_refineSource) {
        auto& mono = _source->getSelectedMonochromator();
        mono.setOffset(_sourceOffset);            
    }

    // Then n parameters for the sample
    auto sgonio=_sample->getGonio();
    if (sgonio) {
        for (unsigned int i = 0; i < sgonio->getNAxes(); ++i) {
            // parameter is fixed: skip it
            if (_refineSample[i]) {
                auto ax = sgonio->getAxis(i);
                ax->setOffset(_sampleOffsets[i]);
            }
        }
    }

    // finally, n parameters for the detector
    auto dgonio=_detector->getGonio();
    if (dgonio)	{
        for (unsigned int i=0;i<dgonio->getNAxes();++i) {
            // parameter is fixed: skip it
            if (_refineDetector[i]) {     
                auto ax = sgonio->getAxis(i);
                ax->setOffset(_detectorOffsets[i]);
            }
        }
    }

    // update the unit cell
    if (_cell) {
        _cell->setReciprocalBasis(ub());
    }
}

void UBSolution::setSample(sptrSample sample)
{
    _sample = sample;
    _nSampleAxes = _sample->hasGonio() ? _sample->getGonio()->getNAxes() : 0;    

    if (_nSampleAxes > 0) {
        _refineSample.resize(_nSampleAxes);
        _sampleOffsets.resize(_nSampleAxes);
        _sigmaSampleOffsets.resize(_nSampleAxes);

        for (int i = 0; i < _nSampleAxes; ++i) {
            _sampleOffsets[i] = _sample->getGonio()->getAxis(i)->getOffset();
            _sigmaSampleOffsets[i] = 0.0;
            _refineSample[i] = false;
        }
    }
}

void UBSolution::setDetector(sptrDetector detector)
{
    _detector = detector;    
    _nDetectorAxes = _detector->hasGonio() ? _detector->getGonio()->getNAxes() : 0;

    if (_nDetectorAxes > 0) {
        _refineDetector.resize(_nDetectorAxes);
        _detectorOffsets.resize(_nDetectorAxes);
        _sigmaDetectorOffsets.resize(_nDetectorAxes);

        for (int i = 0; i < _nDetectorAxes; ++i) {
            _detectorOffsets[i] = _detector->getGonio()->getAxis(i)->getOffset();
            _sigmaDetectorOffsets[i] = 0.0;
            _refineDetector[i] = false;
        }
    }
}

void UBSolution::setSource(sptrSource source)
{
    _source = source;
    _sourceOffset = _source->getSelectedMonochromator().getOffset();
    _sigmaSourceOffset = 0.0;
    _refineSource = false;
}

const Eigen::VectorXd& UBSolution::sampleOffsets() const
{
    return _sampleOffsets;
}

const Eigen::VectorXd& UBSolution::detectorOffsets() const
{
    return _detectorOffsets;
}

const Eigen::VectorXd& UBSolution::sigmaSampleOffsets() const
{
    return _sigmaSampleOffsets;
}

const Eigen::VectorXd& UBSolution::sigmaDetectorOffsets() const
{
    return _sigmaDetectorOffsets;
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

const Eigen::Matrix<double, 9, 9>& UBSolution::covub() const
{
    return _covub;
}

void UBSolution::setCell(sptrUnitCell cell)
{
    _cell = cell;
    _NP = cell->niggliTransformation();
    _uOffsets.setZero();
    _u0 = cell->busingLevyU();
    _covub = Eigen::Matrix<double, 9, 9>::Zero();

    Eigen::Matrix3d A = cell->basis()*_NP.inverse();
    Eigen::Matrix3d G = A.transpose()*A;

    _character0.resize(6);
    _character0 << G(0,0), G(1,1), G(2,2), G(1,2), G(0,2), G(0,1);
    _character = _character0;
}

void UBSolution::update(const Eigen::VectorXd& x, const Eigen::MatrixXd& cov)
{
    unzip(x);

    assert(cov.rows() >= 9);
    assert(cov.cols() >= 9);

    _covub = cov.block(0,0,9,9);
    auto fixed = fixedParameters();

    auto get_sigma = [this, &cov, &fixed] () -> double
    {
        static unsigned int idx = 9;

        if (fixed[idx]) {
            return 0.0;
        } else {
            const double sigma = std::sqrt(cov(idx, idx));
            ++idx;
            return sigma;
        }
    };

    _sigmaSourceOffset = get_sigma();

    for (auto i = 0; i < _sigmaSampleOffsets.size(); ++i) {
        _sigmaSampleOffsets[i] = get_sigma();
    }

    for (auto i = 0; i < _sigmaDetectorOffsets.size(); ++i) {
        _sigmaDetectorOffsets[i] = get_sigma();
    }
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

std::vector<bool> UBSolution::fixedParameters() const
{
    std::vector<bool> fixed(inputs());

    unsigned int idx = 0;

    // UB components
    for (int i = 0; i < 9; ++i) {
        fixed[idx++] = false;
    }

    // wavelength
    fixed[idx++] = !_refineSource;

    // sample
    for (auto i = 0; i < _refineSample.size(); ++i) {
        fixed[idx++] = !_refineSample[i];
    }

    // detector
    for (auto i = 0; i < _refineDetector.size(); ++i) {
        fixed[idx++] = !_refineDetector[i];
    }

    return fixed;
}

} // end namespace nsx
