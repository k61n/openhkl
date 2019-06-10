//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/Experiment.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_EXPERIMENT_EXPERIMENT_H
#define CORE_EXPERIMENT_EXPERIMENT_H


#include "core/instrument/Diffractometer.h"

namespace nsx {

//! Experiment class, a data type containing a diffractometer and data
//! sets.
class Experiment {
public:
    // Constructors & Destructors
    //! Default constructor (deleted)
    Experiment() = delete;
    Experiment(const Experiment& other);

    //! Construct an empty experiment from a given name and diffractometer
    Experiment(const std::string& name, const std::string& diffractometerName);
    ~Experiment() = default;
    Experiment& operator=(const Experiment& other);

    //! Returns a non-const pointerh  to the diffractometer related to the
    //! experiment
    Diffractometer* diffractometer();

    //! Returns a const pointerh  to the diffractometer related to the experiment
    const Diffractometer* diffractometer() const;

    //! Gets a reference to the data
    const std::map<std::string, sptrDataSet>& data() const;
    //! Gets the pointer to a given data stored in the experiment
    sptrDataSet data(std::string name);

    const std::string& name() const;
    // std::string getName() const;
    //! Sets the name of the experiment
    void setName(const std::string& name);

    //! Add some data to the experiment
    void addData(sptrDataSet data);
    //! Returns true if the experiment has a data
    bool hasData(const std::string& name) const;
    //! Remove a data from the experiment
    void removeData(const std::string& name);

private:
    //! The name of this experiment
    std::string _name;

    //! A pointer to the detector assigned to this experiment
    std::unique_ptr<Diffractometer> _diffractometer;

    //! A map of the data related to the experiment. The keys are the basename of
    //! their corresponding file.
    std::map<std::string, sptrDataSet> _data;
};

} // namespace nsx

#endif // CORE_EXPERIMENT_EXPERIMENT_H
