//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/InstrumentStateSet.h
//! @brief     Defines class InstrumentStateSet
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INSTRUMENT_INSTRUMENTSTATESET_H
#define NSX_CORE_INSTRUMENT_INSTRUMENTSTATESET_H

#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"

namespace nsx {

using sptrDataSet = std::shared_ptr<DataSet>;

class DataSet;

/*! \addtogroup python_api
 *  @{*/

/*! \brief Container for *mutable* instrument states (sample
 * positions/orientation, detector offset and incident wavevector) that are
 * modified during refinement.
 */
class InstrumentStateSet {

 public:
    InstrumentStateSet(DataSet* data, const InstrumentStateList& states);
    InstrumentStateSet(sptrDataSet data);
    InstrumentStateSet(sptrDataSet data, const InstrumentStateList& states);
    InstrumentStateList& instrumentStates() { return _instrument_states; };
    void setDiffractometer();

    std::string name() const { return _name; };
    DataSet* data() const { return _data; };
    unsigned int id() const { return _id; };
    void setId(unsigned int id)
    {
        if (_id == 0)
            _id = id;
    };
    void reset() { _instrument_states.clear(); };
    void setInstrumentStates(const InstrumentStateList& states) { _instrument_states = states; };

 private:
    unsigned int _id;
    std::string _name;
    std::size_t _nframes;
    DataSet* _data;
    InstrumentStateList _instrument_states;
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_INSTRUMENT_INSTRUMENTSTATESET_H
