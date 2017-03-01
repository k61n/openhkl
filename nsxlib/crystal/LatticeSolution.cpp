#include <algorithm>
#include <ostream>

#include "../instrument/Detector.h"
#include "../instrument/Gonio.h"
#include "LatticeSolution.h"
#include "../instrument/RotAxis.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../instrument/TransAxis.h"
#include "../utils/Units.h"

namespace SX
{

namespace Crystal
{

LatticeSolution::LatticeSolution()
: _detector(nullptr),
  _sample(nullptr),
  _source(nullptr),
  _latticeParams(),
  _covLatticeParams(),
  _sourceOffset(0),
  _sigmaSourceOffset(0),
  _fixedParameters()
{
}

LatticeSolution::LatticeSolution(const LatticeSolution& other)
{
    _detector = other._detector;
    _sample = other._sample;
    _source= other._source;
    _latticeParams = other._latticeParams;
    _covLatticeParams = other._covLatticeParams;
    _sourceOffset=other._sourceOffset;
    _sigmaSourceOffset=other._sigmaSourceOffset;
    _detectorOffsets = other._detectorOffsets;
    _sigmaDetectorOffsets = other._sigmaDetectorOffsets;
    _sampleOffsets = other._sampleOffsets;
    _sigmaSampleOffsets = other._sigmaSampleOffsets;
    _fixedParameters = other._fixedParameters;
}

LatticeSolution& LatticeSolution::operator=(const LatticeSolution& other)
{
    if (this != &other)
    {
        _detector = other._detector;
        _sample = other._sample;
        _source = other._source;
        _latticeParams = other._latticeParams;
        _covLatticeParams = other._covLatticeParams;
        _sourceOffset= other._sourceOffset;
        _sigmaSourceOffset =other. _sigmaSourceOffset;
        _detectorOffsets = other._detectorOffsets;
        _sigmaDetectorOffsets = other._sigmaDetectorOffsets;
        _sampleOffsets = other._sampleOffsets;
        _sigmaSampleOffsets = other._sigmaSampleOffsets;
        _fixedParameters = other._fixedParameters;
    }
    return *this;
}

LatticeSolution::LatticeSolution(std::shared_ptr<Instrument::Detector> detector,
                                 std::shared_ptr<Instrument::Sample> sample,
                                 std::shared_ptr<Instrument::Source> source,
                                 const Eigen::VectorXd& values,
                                 const Eigen::MatrixXd& cov,
                                 const std::vector<bool>& fixedParameters)
: _detector(detector),
  _sample(sample),
  _source(source),
  _latticeParams(values),
  _fixedParameters(fixedParameters)
{

    int nFixedLatticeParams(std::count(_fixedParameters.begin(),_fixedParameters.begin()+9,true));
    int nFreeLatticeParams = 9-nFixedLatticeParams;

    _covLatticeParams = cov.block(0,0,nFreeLatticeParams,nFreeLatticeParams);

    _sourceOffset=values(nFreeLatticeParams);

    unsigned int idx = nFreeLatticeParams+1;

    std::size_t nSampleAxes = _sample->hasGonio() ? _sample->getGonio()->getNAxes() : 0;
    _sampleOffsets = values.segment(idx,nSampleAxes);
    _sigmaSampleOffsets = Eigen::VectorXd(nSampleAxes);

    idx+=nSampleAxes;
    std::size_t nDetectorAxes= _detector->hasGonio() ? _detector->getGonio()->getNAxes() : 0;
    _detectorOffsets = values.segment(idx,nDetectorAxes);
    _sigmaDetectorOffsets = Eigen::VectorXd(nDetectorAxes);

    idx = nFreeLatticeParams;

    auto& mono = _source->getSelectedMonochromator();
    if (mono.isOffsetFixed()) {
        _sigmaSourceOffset=0.0;
    } else {
        _sigmaSourceOffset=sqrt(cov(nFreeLatticeParams,nFreeLatticeParams));
        ++idx;
    }


    for (unsigned int i=0;i<nSampleAxes;++i) {
        if (_sample->getGonio()->getAxis(i)->hasOffsetFixed()) {
            _sigmaSampleOffsets[i] = 0.0;
        } else {
            _sigmaSampleOffsets[i]=sqrt(cov(idx,idx));
            idx++;
        }
    }

    for (unsigned int i=0;i<nDetectorAxes;++i) {
        if (_detector->getGonio()->getAxis(i)->hasOffsetFixed()) {
            _sigmaDetectorOffsets[i] = 0.0;
        } else {
            _sigmaDetectorOffsets[i]=sqrt(cov(idx,idx));
            idx++;
        }
    }
}

std::ostream& operator<<(std::ostream& os, const LatticeSolution& solution)
{

    if (!(solution._source && solution._sample && solution._detector))
    {
        os <<"No valid solution found."<<std::endl;
        return os;
    }

    os<<"Lattice parameters:"<<std::endl;
    os<<solution._latticeParams.transpose()<< std::endl;

    auto& mono = solution._source->getSelectedMonochromator();

    os << "Wavelength:" << mono.getWavelength() << "("<< solution._sigmaSourceOffset<< ")" << std::endl;

    os<<"Detector offsets: " << std::endl;
    auto detectorG=solution._detector->getGonio();
    for (unsigned int i=0;i<detectorG->getNAxes();++i)
    {
        os << detectorG->getAxis(i)->getLabel() << " ";
        SX::Instrument::Axis* axis=detectorG->getAxis(i);
        if (dynamic_cast<SX::Instrument::TransAxis*>(axis))
            os << solution._detectorOffsets[i]/SX::Units::mm << "(" << solution._sigmaDetectorOffsets[i]/SX::Units::mm << ") mm " << std::endl;
        else if (dynamic_cast<SX::Instrument::RotAxis*>(axis))
            os << solution._detectorOffsets[i]/SX::Units::deg << "(" << solution._sigmaDetectorOffsets[i]/SX::Units::deg << ") deg " << std::endl;
    }
    os <<std::endl;

    os<<"Sample offsets:" << std::endl;
    auto sampleG=solution._sample->getGonio();
    for (unsigned int i=0;i<sampleG->getNAxes();++i)
    {
        os << sampleG->getAxis(i)->getLabel() << " ";
        SX::Instrument::Axis* axis=sampleG->getAxis(i);
        if (dynamic_cast<SX::Instrument::TransAxis*>(axis))
            os << solution._sampleOffsets[i]/SX::Units::mm << "(" << solution._sigmaSampleOffsets[i]/SX::Units::mm << ") mm " << std::endl;
        else if (dynamic_cast<SX::Instrument::RotAxis*>(axis))
            os << solution._sampleOffsets[i]/SX::Units::deg << "(" << solution._sigmaSampleOffsets[i]/SX::Units::deg << ") deg " << std::endl;
    }
    os<<std::endl;

    return os;
}

} // end namespace Crystal

} // end namespace SX
