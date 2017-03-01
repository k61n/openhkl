/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent Chapon, Eric Pelligrini, Jonathan Fisher

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

#include "InstrumentState.h"
#include "Component.h"

namespace SX {
namespace Instrument {

InstrumentState InstrumentState::interpolate(const InstrumentState &other, double t) const
{
    using dvec = std::vector<double>;

    auto interpolate_vec = [] (const dvec& u, const dvec& v, double t) -> dvec {
        assert(u.size() == v.size());
        dvec w(u);
        for (auto i = 0; i < w.size(); ++i) {
            w[i] += t*(v[i]-u[i]);
        }
        return w;
    };

    t = std::max(t, 0.0);
    t = std::min(t, 1.0);

    const auto& detectorState = interpolate_vec(detector.getValues(), other.detector.getValues(), t);
    const auto& sourceState = interpolate_vec(source.getValues(), other.source.getValues(), t);
    const auto& sampleState = interpolate_vec(sample.getValues(), other.sample.getValues(), t);

    InstrumentState result(*this);

    result.detector._values = detectorState;
    result.sample._values = sampleState;
    result.source._values = sourceState;

    return result;
}

} // end namespace Instrument
} // end namespace SX
