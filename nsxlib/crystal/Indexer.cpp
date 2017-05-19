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

#include "Indexer.h"

namespace nsx {

Indexer::Indexer(ptrUnitCell cell):
		_cell(cell),
		_dtol(0.01),
		_angletol(0.01)
{
	auto hkls = _cell->generateReflectionsInSphere(2.0);

    _possiblePeaks.resize(hkls.size());
	auto UB = _cell->getReciprocalStandardM().transpose();

    #pragma omp parallel for
    for (size_t i = 0; i < hkls.size(); ++i) {
        double qnorm = (UB*hkls[i]).norm();
        _possiblePeaks[i].first = qnorm;
        _possiblePeaks[i].second = hkls[i];
    }
}

Indexer::Indexer(const Indexer& other)
: _cell(other._cell),
  _dtol(other._dtol),
  _angletol(other._angletol),
  _peaks(other._peaks),
  _possiblePeaks(other._possiblePeaks)
{
}

Indexer::~Indexer() {
}

Indexer& Indexer::operator=(const Indexer& other) {
    if (this!=&other) {
		_cell = other._cell;
		_dtol = other._dtol;
		_angletol = other._angletol;
		_peaks = other._peaks;
		_possiblePeaks = other._possiblePeaks;
	}

	return *this;
}

std::vector<Eigen::Vector3d> Indexer::index(const Peak3D& peak) {

    std::vector<std::pair<double, Eigen::Vector3d>> remaining_peaks;
    std::vector<Eigen::Vector3d> hkls;

    remaining_peaks.reserve(100);

    auto thisQ = peak.getQ();
    const double this_d_star = thisQ.norm();
    const double lower_bound = this_d_star * (1.0 - _dtol);
    const double upper_bound = this_d_star * (1.0 + _dtol);

    // First exclude by d* comparison
    for (int i = 0; i < _possiblePeaks.size(); ++i) {
        if ( _possiblePeaks[i].first < lower_bound)
            continue;
        if (_possiblePeaks[i].first > upper_bound)
            continue;

        remaining_peaks.push_back(_possiblePeaks[i]);
    }

    using pair_type = std::pair<double, Eigen::Vector3d>;
    auto compare_fn = [](const pair_type& a, const pair_type& b) -> bool {
        return a.first < b.first;
    };

    std::sort(remaining_peaks.begin(), remaining_peaks.end(), compare_fn);

    // now remove additional peaks
    for (int i = 0; i < remaining_peaks.size(); ++i) {
        std::pair<double, Eigen::Vector3d>* it = &remaining_peaks[i];

        //First peak just compare d*
        if (!_peaks.size()) {
            hkls.push_back((*it).second);
        }
        // Need to compare angles with previously stored peaks
        else {
            for (auto old: _peaks) {
                auto oldQ = old->getQ();
                double angle = acos(thisQ.dot(oldQ)/this_d_star/oldQ.norm());
                Eigen::RowVector3d hkl;
                bool success =old->getMillerIndices(hkl,true);
                double angle2 = _cell->getAngle((*it).second, hkl);

                if (angle>(1.0-_angletol)*angle2 && angle<(1.0+_angletol)*angle2) {
                    hkls.push_back((*it).second);
                }
            }
        }
    }

    return hkls;

}

void Indexer::storePeak(Peak3D* peak) {
	_peaks.push_back(peak);
    if (_peaks.size()==2) {
	}
}

} // end namespace nsx

