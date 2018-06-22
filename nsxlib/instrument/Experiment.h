/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "DataTypes.h"
#include "InstrumentTypes.h"

namespace nsx {

//! \brief Experiment class, a data type containing a diffractometer and data sets.
class Experiment {
public:

    // Constructors & Destructors
    //! Default constructor (deleted)
    Experiment()=delete;

    //! Copy constructor
    Experiment(const Experiment& other)=default;

    //! Construct an empty experiment from a given name and diffractometer
    Experiment(const std::string& name, const std::string& diffractometerName);

    //! Construct an empty experiment from a diffractometer name
    Experiment(const std::string& diffractometerName);

    //! Destructor
    virtual ~Experiment();

    //! Assignment operator
    Experiment& operator=(const Experiment& other)=default;

    //! Gets a shared pointer to the diffractometer related to the experiment
    sptrDiffractometer diffractometer() const;

    //! Get a reference to the data
    const std::map<std::string,sptrDataSet>& data() const;
    //! Gets the pointer to a given data stored in the experiment
    sptrDataSet data(std::string name);

    const std::string& name() const;
    //std::string getName() const;
    //! Sets the name of the experiment
    void setName(const std::string& name);

    //! Add some data to the experiment
    void addData(sptrDataSet data);
    //! Check whether the experiment has a data
    bool hasData(const std::string& name) const;
    //! Remove a data from the experiment
    void removeData(const std::string& name);

private:

    //! The name of this experiment
    std::string _name;

    //! The name of the diffractometer assigned to the experiment
    std::string _diffractometerName;

    //! A pointer to the detector assigned to this experiment
    sptrDiffractometer _diffractometer;

    //! A map of the data related to the experiment. The keys are the basename of their corresponding file.
    std::map<std::string,sptrDataSet> _data;
};

} // end namespace nsx
