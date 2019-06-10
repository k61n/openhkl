//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InstrumentTypes.h
//! @brief     Defines ###THINGS###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_INSTRUMENTTYPES_H
#define CORE_INSTRUMENT_INSTRUMENTTYPES_H

#include <memory>
#include <vector>

namespace nsx {

class DetectorEvent;
class Experiment;
class InstrumentState;
class RotAxis;
using sptrExperiment = std::shared_ptr<Experiment>;

using InstrumentStateList = std::vector<InstrumentState>;

} // namespace nsx

#endif // CORE_INSTRUMENT_INSTRUMENTTYPES_H
