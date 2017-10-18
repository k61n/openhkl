#include <stdexcept>

#include <Eigen/Dense>

#include "DirectVector.h"

namespace nsx {

DirectVector::DirectVector(const Eigen::Vector3d& dvector) : _dvector(dvector)
{
}

DirectVector::operator const Eigen::Vector3d& () const
{
    return _dvector;
}

double& DirectVector::operator[](unsigned int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _dvector[index];
}

double& DirectVector::operator()(unsigned int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _dvector[index];
}

void DirectVector::print(std::ostream& os) const
{
    os << _dvector;
}

std::ostream& operator<<(std::ostream& os, const DirectVector& dvector)
{
    dvector.print(os);
    return os;
}


double DirectVector::get2Theta(const Eigen::Vector3d& si) const
{
    Eigen::Vector3d p = _dvector;
    double proj = p.dot(si);
    return acos(proj/p.norm()/si.norm());
}

Eigen::Vector3d DirectVector::getKf(double wave, const Eigen::Vector3d& from) const
{
    // Get the event position x,y,z, taking into account the Gonio current setting
    Eigen::Vector3d p = _dvector - from;
    p.normalize();
    return (p/wave);
}

Eigen::Vector3d DirectVector::getQ(double wave, const Eigen::Vector3d& from) const
{
    if (wave<=0) {
        throw std::runtime_error("Detector:getQ incident wavelength error, must be >0");
    }
    Eigen::Vector3d q = getKf(wave, from);
    q[1]-=1.0/wave;
    return q;
}

void DirectVector::getGammaNu(double& gamma, double& nu, const Eigen::Vector3d& from) const
{
    Eigen::Vector3d p = _dvector-from;
    gamma=std::atan2(p[0],p[1]);
    nu=std::asin(p[2]/p.norm());
}

double DirectVector::getLorentzFactor(const Eigen::Vector3d& from) const
{
    double gamma,nu;
    getGammaNu(gamma, nu, from);
    double lorentz = 1.0/(sin(std::fabs(gamma))*cos(nu));
    return lorentz;
}

} // end namespace nsx
