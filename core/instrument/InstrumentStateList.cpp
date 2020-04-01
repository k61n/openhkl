//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InstrumentStateList.cpp
//! @brief     Implements class InstrumentStateList
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/InstrumentStateList.h"

namespace nsx {

InterpolatedState InstrumentStateList::interpolate(const double frame) const
{
    if (frame > (size() - 1) || frame < 0)
        throw std::runtime_error(
            "Error when interpolating state: invalid frame value: " + std::to_string(frame));

    const std::size_t idx = std::size_t(std::lround(std::floor(frame)));
    const std::size_t next = std::min(idx + 1, size() - 1);

    return InterpolatedState(at(idx), at(next), frame - idx);
}

} // namespace nsx
