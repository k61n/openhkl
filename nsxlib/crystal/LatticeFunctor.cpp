#include <cmath>

#include <Eigen/Geometry>

#include "../instrument/Detector.h"
#include "../kernel/Error.h"
#include "../instrument/Gonio.h"
#include "LatticeFunctor.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../crystal/UnitCell.h"
#include "../crystal/Peak3D.h"

namespace nsx
{

namespace Crystal
{

LatticeFunctor::LatticeFunctor()
: Utils::LMFunctor<double>(),
  _peaks(0),
  _detector(nullptr),
  _sample(nullptr),
  _source(nullptr),
  _constraints(),
  _constants()
{
}

LatticeFunctor::LatticeFunctor(const LatticeFunctor& other) : Utils::LMFunctor<double>(other)
{
    _peaks = other._peaks;
    _detector = other._detector;
    _sample = other._sample;
    _source = other._source;
    _constraints = other._constraints;
    _constants = other._constants;
}

LatticeFunctor& LatticeFunctor::operator=(const LatticeFunctor& other)
{
    if (this != &other)
    {
        Utils::LMFunctor<double>::operator=(other);
        _peaks = other._peaks;
        _detector = other._detector;
        _sample = other._sample;
        _source = other._source;
        _constraints = other._constraints;
        _constants = other._constants;
    }
    return *this;
}

LatticeFunctor::~LatticeFunctor()
{
}

int LatticeFunctor::operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
{
    if (!_detector || !_sample || !_source)
        throw nsx::Kernel::Error<LatticeFunctor>("A detector, sample and source must be specified prior to calculate residuals.");

    Eigen::VectorXd xlocal(x);

    // Set the fixed cell parameters
    for (const auto& p : _constants)
        xlocal[p.first] = p.second;

    // Set the cell constraints
    for (const auto& p : _constraints)
    {
        int lhs=std::get<0>(p);
        int rhs=std::get<1>(p);
        double factor=std::get<2>(p);
        xlocal[lhs] = factor*xlocal[rhs];
    }

    UnitCell uc(xlocal[0],xlocal[1],xlocal[2],xlocal[3],xlocal[4],xlocal[5]);

    Eigen::Matrix3d B=uc.getBusingLevyB();

    // The parameters 6,7,8 are the three angles that will define U matrix through quaternion formalism
    // 6 = phi angle of spherical coordinates
    // 7 = theta angle of spherical coordinates
    // 8 = omega angle of quaternion
    double sThetacPhi=sin(xlocal[6])*cos(xlocal[7]);
    double sThetasPhi=sin(xlocal[6])*sin(xlocal[7]);
    double cPhi = cos(xlocal[6]);
    double omegaOver2 = xlocal[8]/2.0;
    double cOmegaOver2 = cos(omegaOver2);
    double sOmegaOver2 = sin(omegaOver2);
    Eigen::Quaterniond quat(cOmegaOver2,sOmegaOver2*sThetacPhi,sOmegaOver2*sThetasPhi,sOmegaOver2*cPhi);
    Eigen::Matrix3d U(quat.toRotationMatrix());
    U.transposeInPlace();

    Eigen::Matrix3d BU = B*U;

    // The other parameters are for the instrument offset
    int naxes=9;

    // Parameter 9 is offset in wavelength
    auto& mono = _source->getSelectedMonochromator();
    mono.setOffset(xlocal[naxes++]);

    // Then n parameters for the detector
    auto sgonio=_sample->getGonio();
    if (sgonio)
    {
        for (unsigned int i=0;i<sgonio->getNAxes();++i)
            sgonio->getAxis(i)->setOffset(xlocal[naxes++],true);
    }

    // finally, n parameters for the sample
    auto dgonio=_detector->getGonio();
    if (dgonio)
    {
        for (unsigned int i=0;i<dgonio->getNAxes();++i)
            dgonio->getAxis(i)->setOffset(xlocal[naxes++],true);
    }

    for (unsigned int i=0; i<_peaks.size();++i)
    {
        Eigen::RowVector3d qVector=_peaks[i].getQ();
        Eigen::RowVector3d hkl;
        bool success=_peaks[i].getMillerIndices(hkl,true);
        fvec(3*i)   = (BU(0,0)*hkl[0] + BU(0,1)*hkl[1] + BU(0,2)*hkl[2] - qVector[0]);
        fvec(3*i+1) = (BU(1,0)*hkl[0] + BU(1,1)*hkl[1] + BU(1,2)*hkl[2] - qVector[1]);
        fvec(3*i+2) = (BU(2,0)*hkl[0] + BU(2,1)*hkl[1] + BU(2,2)*hkl[2] - qVector[2]);
    }

    return 0;
}

int LatticeFunctor::inputs() const
{
    // 10 = 6 lattice parameters + 3 U matrix quaternion parameters + wavelength
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

int LatticeFunctor::values() const
{
    return 3*_peaks.size();
}

} // namespace Crystal
} // end namespace nsx
