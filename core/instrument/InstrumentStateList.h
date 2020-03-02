//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InstrumentStateList.h
//! @brief     Defines class InstrumentStateList
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_INSTRUMENTSTATELIST_H
#define CORE_INSTRUMENT_INSTRUMENTSTATELIST_H

#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h"
#include <vector>

namespace nsx {

class InstrumentStateList : public std::vector<InstrumentState> {
 public:
    InterpolatedState interpolate(const double frame) const;
};


} // namespace nsx

#endif // CORE_INSTRUMENT_INSTRUMENTSTATELIST_H
