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

#pragma once

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>

#include "Component.h"
#include "DirectVector.h"
#include "Enums.h"
#include "GeometryTypes.h"

namespace nsx {

//! Lightweight class for detector events.
class DetectorEvent {
public:
  //! Constructor
  DetectorEvent(double px = 0, double py = 0, double frame = -1,
                double tof = -1);

  //! Construct from a 3 vector
  explicit DetectorEvent(Eigen::Vector3d x, double tof = -1);

  //! Detector x-coord
  double _px;
  //! Detector y-coord
  double _py;
  //! Frame number, if applicable
  double _frame;
  //! Time of flight, if applicable
  double _tof;
};

} // end namespace nsx
