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

#ifndef NSXTOOL_DIFFRACTOMETER_H_
#define NSXTOOL_DIFFRACTOMETER_H_

#include <map>
#include <string>
#include <memory>

#include <boost/property_tree/ptree.hpp>

namespace nsx {

namespace property_tree=boost::property_tree;

// Forwards
class Detector;
class Sample;
class Source;

class Diffractometer {
public:

    Diffractometer(const property_tree::ptree& node);

    //! Constructs a diffractometer from another one
    Diffractometer(const Diffractometer& other);

    //! Virtual copy constructor
    virtual Diffractometer* clone() const {return nullptr;}

    //! Destructor
    virtual ~Diffractometer();

    //! Assignment operator
    Diffractometer& operator=(const Diffractometer& other);

    //! Get the name of this diffractometer
    const std::string& getName() const;

    //! Get the detector of this diffractometer
    std::shared_ptr<Detector> getDetector();

    //! Get the sample of this diffractometer
    std::shared_ptr<Sample> getSample();

    //! Get the source of this diffractometer
    std::shared_ptr<Source> getSource();

    //! Get the type of the diffractometer
    virtual std::string getType() const {return "";}

    //! Set the detector of this diffractometer
    void setDetector(std::shared_ptr<Detector>);

    //! Set the name of the diffractometer
    void setName(const std::string& name);

    //! Set the sample of this diffractometer
    void setSample(std::shared_ptr<Sample>);

    //! Set the source of this diffractometer
    void setSource(std::shared_ptr<Source>);

    std::map<unsigned int,std::string> getPhysicalAxesNames() const;

protected:

    //! Default constructor
    Diffractometer();

    //! Constructs a diffractometer with a given name
    Diffractometer(const std::string& name);

    std::string _name;
    std::shared_ptr<Detector> _detector;
    std::shared_ptr<Sample> _sample;
    std::shared_ptr<Source> _source;
};

} // end namespace nsx

#endif /* NSXTOOL_DIFFRACTOMETER_H_ */
