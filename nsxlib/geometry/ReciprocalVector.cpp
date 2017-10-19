#include <Eigen/Dense>

#include "ReciprocalVector.h"

namespace nsx {

ReciprocalVector::ReciprocalVector(const Eigen::RowVector3d& rvector) : _rvector(rvector)
{
}

ReciprocalVector::operator const Eigen::RowVector3d& () const
{
    return _rvector;
}

double& ReciprocalVector::operator[](unsigned int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _rvector[index];
}

double& ReciprocalVector::operator()(unsigned int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _rvector[index];
}

void ReciprocalVector::print(std::ostream& os) const
{
    os << _rvector;
}

std::ostream& operator<<(std::ostream& os, const ReciprocalVector& rvector)
{
    rvector.print(os);
    return os;
}

void ReciprocalVector::getGammaNu(double& gamma, double& nu) const
{
    gamma=std::atan2(_rvector[0],_rvector[1]);
    nu=std::asin(_rvector[2]/_rvector.norm());
}

double ReciprocalVector::getLorentzFactor() const
{
    double gamma,nu;
    getGammaNu(gamma, nu);
    double lorentz = 1.0/(sin(std::fabs(gamma))*cos(nu));
    return lorentz;
}

double ReciprocalVector::get2Theta(const ReciprocalVector& q, const ReciprocalVector& ki)
{
    auto kf = ki._rvector + q._rvector;
    double proj = kf.dot(ki._rvector);
    return acos(proj/kf.norm()/ki._rvector.norm());
}

} // end namespace nsx
