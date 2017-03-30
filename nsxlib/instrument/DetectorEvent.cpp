/*
 nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Institut Laue-Langevin
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 -----------------------------------------------------------------------------------------

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

#include "Detector.h"
#include "DetectorEvent.h"
#include "Gonio.h"

namespace SX {
namespace Instrument {


DetectorEvent::DetectorEvent(const Detector& detector, double x, double y, std::vector<double> values):
    _detector(&detector), _x(x), _y(y), _values(std::move(values))
{

}

DetectorEvent::DetectorEvent(const DetectorEvent& rhs):_detector(rhs._detector),_x(rhs._x),_y(rhs._y),_values(rhs._values)
{

}

DetectorEvent::DetectorEvent(DetectorEvent&& other):
    _detector(other._detector),
    _x(other._x),
    _y(other._y),
    _values(std::move(other._values))
{

}

DetectorEvent& DetectorEvent::operator=(const DetectorEvent& rhs)
{
    _detector = rhs._detector;
    _x = rhs._x;
    _y = rhs._y;
    return *this;
}

DetectorEvent::~DetectorEvent()
{
}

double DetectorEvent::get2Theta(const Eigen::Vector3d& si) const
{
    Eigen::Vector3d p = getPixelPosition();
    double proj = p.dot(si);
    return acos(proj/p.norm()/si.norm());
}

Eigen::Vector3d DetectorEvent::getKf(double wave, const Eigen::Vector3d& from) const
{
    // Get the event position x,y,z, taking into account the Gonio current setting
    Eigen::Vector3d p = getPixelPosition();
    p-=from;
    p.normalize();
    return (p/wave);
}

Eigen::Vector3d DetectorEvent::getQ(double wave, const Eigen::Vector3d& from) const
{
    if (wave<=0) {
        throw std::runtime_error("Detector:getQ incident wavelength error, must be >0");
    }
    Eigen::Vector3d q = getKf(wave, from);
    q[1]-=1.0/wave;
    return q;
}

void DetectorEvent::getGammaNu(double& gamma, double& nu, const Eigen::Vector3d& from) const
{
    Eigen::Vector3d p = getPixelPosition()-from;
    gamma=std::atan2(p[0],p[1]);
    nu=std::asin(p[2]/p.norm());
}

Eigen::Vector3d DetectorEvent::getPixelPosition() const
{
    Eigen::Vector3d v = _detector->getPos(_x, _y);
    auto gonio = _detector->getGonio();

    // No gonio and no values set
    if (gonio == nullptr) {
        if ( _values.size()) {
            throw std::runtime_error("Trying to assign a DetectorEvent with values to a Component with no Goniometer");
        }
        return v;
    }
    if (gonio->getNPhysicalAxes() != _values.size()) {
        throw std::runtime_error("Trying to assign a DetectorEvent with wrong number of values");
    }
    gonio->transformInPlace(v, _values);
    return v;
}

} /* namespace Instrument */
} // Namespace SX
