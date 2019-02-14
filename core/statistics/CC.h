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

#pragma once

#include "MergedPeak.h"

namespace nsx {
class MergedData;

//! Class to handle calculation of correlation coefficients (CChalf and CC*)
class CC {
public:
  //! Default constructor
  CC();
  //! Calculate the statistic on the given set of merged peaks.
  void calculate(const std::vector<MergedPeak> &peaks);
  //! Calculate the statistic on the given data.
  void calculate(const MergedData &data);
  //! Return CC half (correlation of a random split of data into two bags)
  double CChalf() const;
  //! Return CC star (estimate)
  double CCstar() const;
  //! Return number of peaks
  unsigned int nPeaks() const;

private:
  double _CChalf;
  double _CCstar;
  unsigned int _nPeaks;
};

} // end namespace nsx
