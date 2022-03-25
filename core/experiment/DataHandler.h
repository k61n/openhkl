//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef NSX_CORE_EXPERIMENT_DATAHANDLER_H
#define NSX_CORE_EXPERIMENT_DATAHANDLER_H

#include "core/data/DataTypes.h"
#include "core/experiment/InstrumentStateHandler.h"
#include <map>
#include <stdexcept>
#include <string>

namespace nsx {

using DataMap = std::map<std::string, sptrDataSet>;

class DataSet;
class Diffractometer;
class InstrumentStateHandler;

class DataHandler {

 public:
    DataHandler() = default;
    ~DataHandler() = default;
    DataHandler(const DataHandler& other) = delete;
    DataHandler& operator=(const DataHandler& other) = delete;
    DataHandler(
        const std::string& experiment_name, const std::string& diffractometerName,
        InstrumentStateHandler* instrument_state_handler);

 public: // Handling data sets
    //! Get the diffractometer
    Diffractometer* getDiffractometer();
    //! Set the diffractometer
    void setDiffractometer(const std::string& diffractometerName);
    //! Gets a reference to the data
    const DataMap* getDataMap() const;
    //! Gets the pointer to a given data stored in the experiment
    sptrDataSet getData(std::string name) const;
    //! Return all data sets as a DataList
    DataList getAllData() const;
    //! Get number of data
    int numData() const;
    //! Add some data to the experiment
    bool addData(sptrDataSet data, std::string name);
    //! Returns true if the experiment has a data
    bool hasData(const std::string& name) const;
    //! Remove a data from the experiment
    void removeData(const std::string& name);

 private:
    std::string _experiment_name;
    std::unique_ptr<Diffractometer> _diffractometer;
    //! A map of the data related to the experiment.
    DataMap _data_map;
    //! pointer to handler for instrument states
    InstrumentStateHandler* _instrument_state_handler;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_DATAHANDLER_H
