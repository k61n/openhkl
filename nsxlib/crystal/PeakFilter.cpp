#include "PeakFilter.h"
#include "Peak3D.h"

namespace nsx {

PeakFilter::PeakFilter()
{

}

PeakFilter::~PeakFilter()
{

}

void PeakFilter::filter(sptrDataSet data) const {

}

bool PeakFilter::invalid(sptrDataSet data, sptrPeak3D peak) const
{
    if (_removeUnindexed) {
        // todo
    }
   
    if (_removeMasked) {
        // todo
    }

    if (_removeUnselected) {
        if (!peak->isSelected()) {
            return true;
        }
    }

    if (_removeMultiplyIndexed) {
        // todo
    }

    if (_removeIsigma) {
        Intensity i = peak->getCorrectedIntensity();
        if (i.getValue() / i.getSigma() < _Isigma) {
            return true;
        }
    }

    if (_removeSignificance) {
        // todo
    }

    if (_removeOverlapping) {
        // todo
    }

    const double d = 1.0 / peak->getQ().norm();

    if (_removeDmin) {
        if (d < _dmin) {
            return true;
        }
    }

    if (_removeDmax) {
        if (d > _dmax) {
            return true;
        }
    }

    // failed to violate any condition
    return false;
}

} // end namespace nsx
