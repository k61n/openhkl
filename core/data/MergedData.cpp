/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
 *
 */

#include "MergedData.h"

namespace nsx {

MergedData::MergedData(const SpaceGroup &grp, bool friedel)
    : _group(grp), _friedel(friedel), _peaks() {}

bool MergedData::addPeak(const sptrPeak3D &peak) {
  MergedPeak new_peak(_group, _friedel);
  new_peak.addPeak(peak);
  auto it = _peaks.find(new_peak);

  if (it != _peaks.end()) {
    MergedPeak merged(*it);
    merged.addPeak(peak);
    _peaks.erase(it);
    _peaks.emplace(std::move(merged));
    return false;
  }
  _peaks.emplace(std::move(new_peak));
  return true;
}

const MergedPeakSet &MergedData::peaks() const { return _peaks; }

size_t MergedData::totalSize() const {
  size_t total = 0;

  for (const auto &peak : _peaks) {
    total += peak.redundancy();
  }
  return total;
}

double MergedData::redundancy() const {
  return double(totalSize()) / double(_peaks.size());
}

void MergedData::clear() { _peaks.clear(); }

} // end namespace nsx
