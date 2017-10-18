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

namespace nsx {




DetectorEvent::DetectorEvent(const Detector* detector, double x, double y, double t, std::vector<double> values):
    _detector(detector), _x(x), _y(y), _t(t)
{

}

DetectorEvent::DetectorEvent(const DetectorEvent& rhs):_detector(rhs._detector),_x(rhs._x),_y(rhs._y), _t(rhs._t)
{

}

DetectorEvent::DetectorEvent(DetectorEvent&& other):
    _detector(other._detector),
    _x(other._x),
    _y(other._y),
    _t(other._t)
{

}

DetectorEvent& DetectorEvent::operator=(const DetectorEvent& rhs)
{
    _detector = rhs._detector;
    _x = rhs._x;
    _y = rhs._y;
    _t = rhs._t;
    return *this;
}

DetectorEvent::~DetectorEvent()
{
}


  Eigen::Vector3d DetectorEvent::detectorPosition() const
  {
      return {_x, _y, _t};
  }

} // end namespace nsx
