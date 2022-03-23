//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/InstrumentStateHandler.h
//! @brief     Handles peak manipulations for Experiment object
//! //! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_INSTRUMENTSTATEHANDLER_H
#define NSX_CORE_EXPERIMENT_INSTRUMENTSTATEHANDLER_H

#include "core/instrument/InstrumentStateSet.h"

#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InstrumentStateSet.h"

#include <memory>
#include <string>
#include <map>

namespace nsx {

using InstrumentStateMap = std::map<sptrDataSet, std::unique_ptr<InstrumentStateSet>>;

class InstrumentStateHandler {

 public:
    //! Add a set of instrment states
    bool addInstrumentStateSet(sptrDataSet data);
    //! Add a set of instrment states
    bool addInstrumentStateSet(sptrDataSet data, const InstrumentStateList& states);
    //! Add a set of instrment states
    bool addInstrumentStateSet(
        sptrDataSet data, std::unique_ptr<InstrumentStateSet>& states);
    //! Returns true if the experiment has named instrument state set
    bool hasInstrumentStateSet(const sptrDataSet& data) const;
    //! Returns the named InstrumentStateSet
    InstrumentStateSet* getInstrumentStateSet(const sptrDataSet& data);
    //! Returns the named InstrumentStateSet
    InstrumentStateSet* getInstrumentStateSet(const DataSet* data);
        // !Remove a set of instrument states from the experiment
        void removeInstrumentStateSet(const sptrDataSet& data);
    //! Get a vector of InstrumentStateSet names from the handler
    std::vector<std::string> getInstrumentStateSetNames() const;
    //! Get the number of instrument state sets
    std::size_t numInstrumentStateSets() const { return _instrumentstate_map.size(); };
    //! Set the last index on loading an experiment
    void setLastIndex(unsigned int index) { _last_index = index; };

 private:
    //! Container for InstrumentStateSet unique pointers
    InstrumentStateMap _instrumentstate_map;
    //! Index for generating unique id
    unsigned int _last_index = 0;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_INSTRUMENTSTATEHANDLER_H
