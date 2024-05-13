//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/InstrumentStateSet.h
//! @brief     Defines class InstrumentStateSet
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_INSTRUMENTSTATESET_H
#define OHKL_CORE_INSTRUMENT_INSTRUMENTSTATESET_H

#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"

namespace ohkl {

using sptrDataSet = std::shared_ptr<DataSet>;

class DataSet;
class Diffractometer;

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
    InstrumentStateSet(
        Diffractometer* diffractometer, const std::string& name, const std::size_t nframes);
    InstrumentStateList& instrumentStates() { return _instrument_states; };
    void setDiffractometer();

    //! Get the name of the InstrumentStateSet
    std::string name() const { return _name; };
    //! Get a pointer to the associated DataSet
    DataSet* data() const { return _data; };
    //! Get the integer id
    unsigned int id() const { return _id; };
    //! Set the integer id
    void setId(unsigned int id)
    {
        if (_id == 0)
            _id = id;
    };
    //! Delete the instrument states
    void reset() { _instrument_states.clear(); };
    //! Set the states from a vector of InstrumentStates
    void setInstrumentStates(const InstrumentStateList& states) { _instrument_states = states; };
    //! Get the vector of InstrumentStates
    InstrumentStateList getInstrumentStateList() { return _instrument_states; };
    //! Return a single InstrumentState by frame index
    const InstrumentState* state(std::size_t frame);
    //! Apply the direct beam offset specified by the instrument (yml2c) file
    void applyBeamOffset();


 private:
    unsigned int _id;
    std::string _name;
    std::size_t _nframes;
    DataSet* _data;
    InstrumentStateList _instrument_states;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_INSTRUMENTSTATESET_H
