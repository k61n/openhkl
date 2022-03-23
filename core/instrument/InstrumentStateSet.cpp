//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InstrumentStateSet.cpp
//! @brief     Implements class InstrumentStateSet
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/InstrumentStateSet.h"

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"

namespace nsx {

InstrumentStateSet::InstrumentStateSet(DataSet* data, const InstrumentStateList& states)
    : _id(0), _data(data), _instrument_states(states)
{
    _name = data->name();
    _nframes = data->nFrames();
    _instrument_states.reserve(_nframes);
    for (const auto& state : states) {
        _instrument_states.push_back(state);
    }
}

InstrumentStateSet::InstrumentStateSet(sptrDataSet data) : _id(0), _data(data.get())
{
    _name = data->name();
    _nframes = data->nFrames();
    _instrument_states.reserve(_nframes);

    for (unsigned int i = 0; i < _nframes; ++i)
        _instrument_states.push_back(data->diffractometer()->instrumentState(i));
}

InstrumentStateSet::InstrumentStateSet(sptrDataSet data, const InstrumentStateList& states)
    : _id(0), _data(data.get()), _instrument_states(states)
{
    _name = data->name();
    _nframes = data->nFrames();
}

} // namespace nsx
