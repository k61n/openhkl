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

#include "RFactor.h"
#include "Peak3D.h"

using namespace std;

namespace SX {

namespace Crystal {

void RFactor::recalculate(const vector<vector<sptrPeak3D> > &peak_equivs)
{
    _Rmerge = 0;
    _Rmeas = 0;
    _Rpim = 0;

    double I_total = 0.0;

    // go through each equivalence class of peaks
    for (auto&& peak_list: peak_equivs) {
        // skip if there are fewer than two peaks
        if ( peak_list.size() < 2)
            continue;

        double average = 0.0;
        double sigma, var = 0.0;

        for (auto&& p: peak_list) {
            double in = p->getScaledIntensity();
            average += in;
        }

        const double n = peak_list.size();
        average /= n;

        I_total += n*average;

        const double Fmeas = std::sqrt(n / (n-1));
        const double Fpim = std::sqrt(1 / (n-1));

        for (auto&& p: peak_list) {
            double diff = std::fabs(p->getScaledIntensity() - average);
            _Rmerge += diff;
            _Rmeas += Fmeas*diff;
            _Rpim += Fpim*diff;
        }
    }

    if (I_total < 1e-3) {
        // something wrong! too few peaks?
        _Rmerge = 0.0;
        _Rmeas = 0.0;
        _Rpim = 0.0;
    }
    else {
        _Rmerge /= I_total;
        _Rmeas /= I_total;
        _Rpim /= I_total;
    }
}

RFactor::RFactor(const vector<vector<sptrPeak3D> > &peak_equivs): RFactor()
{
    recalculate(peak_equivs);
}

} // namespace Crystal

} // namespace SX
