#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"
#include "../data/DataSet.h"
#include "IndexedPeakValidator.h"
#include "Peak3D.h"
#include "UnitCell.h"

namespace nsx {

IndexedPeakValidator::IndexedPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
}

bool IndexedPeakValidator::isValid(sptrPeak3D peak) const
{
    for (int i = 0; i < peak->getData()->getDiffractometer()->getSample()->getNCrystals(); ++i) {
        nsx::UnitCell cell = *peak->getData()->getDiffractometer()->getSample()->getUnitCell(i);
        Eigen::RowVector3d hkl;
        bool indexingSuccess = peak->getMillerIndices(cell,hkl,true);
        if (indexingSuccess) {
            return true;
        }
    }
    return false;
}

} // end namespace nsx
