//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/InstrumentStateSet.cpp
//! @brief     Implements class InstrumentStateSet
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/InstrumentStateSet.h"

#include "base/geometry/DirectVector.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"

namespace ohkl {

InstrumentStateSet::InstrumentStateSet(DataSet* data, const InstrumentStateList& states)
    : _id(0), _data(data), _instrument_states(states)
{
    ohklLog(
        Level::Info, "InstrumentStateSet::InstrumentStateSet: adding given state list to DataSet");
    _name = data->name();
    _nframes = data->nFrames();
    // _instrument_states.reserve(_nframes);
    for (const auto& state : states) {
        _instrument_states.push_back(state);
    }
}

InstrumentStateSet::InstrumentStateSet(sptrDataSet data) : _id(0), _data(data.get())
{
    ohklLog(
        Level::Info,
        "InstrumentStateSet::InstrumentStateSet: generating default states for DataSet");
    _name = data->name();
    _nframes = data->nFrames();
    // _instrument_states.reserve(_nframes);
    const auto& mono = _data->diffractometer()->source().selectedMonochromator();
    double x_coord = mono.xOffset() + static_cast<double>(_data->nCols() / 2.0);
    double y_coord = mono.yOffset() + static_cast<double>(_data->nRows() / 2.0);
    DirectVector direct = _data->detector().pixelPosition(x_coord, y_coord);
    ohklLog(
        Level::Info, "Adjusting incident wavevector for direct beam at coordinates (",
        x_coord, ", ", y_coord, ")");

    for (unsigned int i = 0; i < _nframes; ++i) {
        InstrumentState state = data->diffractometer()->instrumentState(i);
        state.adjustKi(direct);
        _instrument_states.push_back(state);
    }
}

InstrumentStateSet::InstrumentStateSet(sptrDataSet data, const InstrumentStateList& states)
    : _id(0), _data(data.get()), _instrument_states(states)
{
    ohklLog(Level::Info, "InstrumentStateSet::InstrumentStateSet: adding given states to DataSet");
    _name = data->name();
    _nframes = data->nFrames();
}

InstrumentStateSet::InstrumentStateSet(
    Diffractometer* diffractometer, const std::string& name, const std::size_t nframes)
    : _id(0), _name(name), _nframes(nframes)
{
    for (std::size_t idx = 0; idx < _nframes; ++idx)
        _instrument_states.push_back(diffractometer->instrumentState(idx));
}

void InstrumentStateSet::setDiffractometer()
{
    for (auto& state : _instrument_states)
        state.setDiffractometer(_data->diffractometer());
}

const InstrumentState* InstrumentStateSet::state(std::size_t frame)
{
    if (frame > _instrument_states.size())
        throw std::runtime_error("InstrumentStateSet::state: frame index out of range");
    return &_instrument_states.at(frame);
}

} // namespace ohkl
