//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/DataHandler.h
//! @brief     Handles DataSet manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_DATAHANDLER_H
#define OHKL_CORE_EXPERIMENT_DATAHANDLER_H

#include "core/data/DataTypes.h"
#include "core/experiment/InstrumentStateHandler.h"
#include <map>
#include <stdexcept>
#include <string>

namespace ohkl {

using DataMap = std::map<std::string, sptrDataSet>;

class DataSet;
class Diffractometer;
class InstrumentStateHandler;

//! Holds named DataSet%s, experiment name, and info on the instrument.
class DataHandler {

 public:
    DataHandler() = default;
    ~DataHandler() = default;
    DataHandler(const DataHandler& other) = delete;
    DataHandler& operator=(const DataHandler& other) = delete;
    DataHandler(
        const std::string& experiment_name, const std::string& diffractometerName,
        InstrumentStateHandler* instrument_state_handler);

 public:
    //! Returns pointer to the diffractometer
    Diffractometer* getDiffractometer();
    //! Sets the diffractometer
    void setDiffractometer(const std::string& diffractometerName);
    //! Returns pointer to the DataMap
    const DataMap* getDataMap() const;
    //! Returns pointer to the DataSet of given name
    sptrDataSet getData(std::string name) const;
    //! Returns all data sets as a DataList
    DataList getAllData() const;
    //! Returns number of DataSet%s
    int numData() const;
    //! Adds a DataSet to the experiment
    bool addData(sptrDataSet data, std::string name, bool default_states = true);
    //! Returns true if the experiment has a DataSet of given name
    bool hasData(const std::string& name) const;
    //! Removes a DataSet from the experiment
    void removeData(const std::string& name);

 private:
    std::string _experiment_name;
    std::unique_ptr<Diffractometer> _diffractometer;
    //! Map name -> DataSet, holding all data sets pertaining to the experiment.
    DataMap _data_map;
    //! Pointer to handler for instrument states
    InstrumentStateHandler* _instrument_state_handler;
};

} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_DATAHANDLER_H
