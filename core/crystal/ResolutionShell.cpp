/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include <cmath>
#include <limits>

#include <Eigen/Eigenvalues>

#include "Peak3D.h"
#include "ReciprocalVector.h"
#include "ResolutionShell.h"

namespace nsx {

ResolutionShell::ResolutionShell(double dmin, double dmax, size_t num_shells)
    : _shells(std::max(num_shells, size_t(1))) {
  size_t n_shells = _shells.size();

  const double q3max = std::pow(dmin, -3);
  const double dq3 =
      (std::pow(dmin, -3) - std::pow(dmax, -3)) / double(n_shells);

  _shells[0].dmin = dmin;

  for (size_t i = 0; i < n_shells - 1; ++i) {
    _shells[i].dmax = std::pow(q3max - (i + 1) * dq3, -1.0 / 3.0);
    _shells[i + 1].dmin = _shells[i].dmax;
  }

  _shells[n_shells - 1].dmax = dmax;
}

void ResolutionShell::addPeak(const sptrPeak3D &peak) {
  auto q = peak->q();
  const double d = 1.0 / q.rowVector().norm();

  double dmin;
  double dmax;
  for (size_t i = 0; i < _shells.size(); ++i) {
    dmin = _shells[i].dmin;
    dmax = _shells[i].dmax;
    if (dmin <= d && d <= dmax) {
      _shells[i].peaks.push_back(peak);
      return;
    }
  }
}

const DShell &ResolutionShell::shell(size_t i) const {
  if (i >= _shells.size()) {
    throw std::runtime_error("ResolutionShell::shell index out of bounds");
  }
  return _shells[i];
}

size_t ResolutionShell::nShells() const { return _shells.size(); }

} // end namespace nsx
