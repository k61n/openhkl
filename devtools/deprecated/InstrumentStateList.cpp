//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InstrumentStateList.cpp
//! @brief     Implements class InstrumentStateList
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/InstrumentStateList.h"
#include <stdexcept>

namespace nsx {

InterpolatedState interpolate(InstrumentStateList& states, const double frame_idx)
{
    if (frame_idx > (size() - 2) || frame_idx < 0)
        throw std::range_error(
            "Error when interpolating state: invalid frame index: "
            + std::to_string(frame_idx));

    const std::size_t idx = std::size_t(std::lround(std::floor(frame_idx)));
    const std::size_t next = std::min(idx + 1, size() - 1);

    if (idx == next) // I *think* this only happens on the last frame of the data set - zamaan
        throw std::range_error(
            "InstrumentStateList::interpolate: Attempting to interpolate using 1 frame");

    return InterpolatedState(states.at(idx), states.at(next), frame_idx - idx);
}

} // namespace nsx
