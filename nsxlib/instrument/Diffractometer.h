/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
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

#include <yaml-cpp/yaml.h>

#include "InstrumentTypes.h"

namespace nsx {

class Diffractometer {

public:

    static sptrDiffractometer build(const std::string& name);

public:

    Diffractometer(const YAML::Node& node);

    //! Constructs a diffractometer from another one
    Diffractometer(const Diffractometer& other);

    //! Virtual copy constructor
    virtual Diffractometer* clone() const {return nullptr;}

    //! Destructor
    virtual ~Diffractometer();

    //! Assignment operator
    Diffractometer& operator=(const Diffractometer& other);

    //! Get the name of this diffractometer
    const std::string& name() const;

    //! Get the detector of this diffractometer
    sptrDetector detector();

    //! Get the sample of this diffractometer
    sptrSample sample();

    //! Get the source of this diffractometer
    sptrSource source();

    //! Set the detector of this diffractometer
    void setDetector(sptrDetector detector);

    //! Set the name of the diffractometer
    void setName(const std::string& name);

    //! Set the sample of this diffractometer
    void setSample(sptrSample sample);

    //! Set the source of this diffractometer
    void setSource(sptrSource source);

protected:

    //! Default constructor
    Diffractometer();

    //! Constructs a diffractometer with a given name
    Diffractometer(const std::string& name);

    std::string _name;
    sptrDetector _detector;
    sptrSample _sample;
    sptrSource _source;
};

} // end namespace nsx
