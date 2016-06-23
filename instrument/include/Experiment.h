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

#ifndef NSXTOOL_EXPERIMENT_H_
#define NSXTOOL_EXPERIMENT_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace SX
{

namespace Data
{
class IData;
}

namespace Instrument
{

using namespace SX::Data;

// Forward declarations
class Diffractometer;

class Experiment
{
public:

	// Constructors & Destructors

	//! Default constructor (deleted)
	Experiment()=delete;
	//! Copy constructor
	Experiment(const Experiment& other);
	//! Construct an empty experiment from a given name and diffractometer
	Experiment(const std::string& name, const std::string& diffractometerName);
	//! Construct an empty experiment from a diffractometer name
	Experiment(const std::string& diffractometerName);
	//! Destructor
	virtual ~Experiment();

	// Operators

	//! Assignment operator
	Experiment& operator=(const Experiment& other);

	// Getters and setters

	//! Gets the type of the diffractomer used in the experiment
	const std::string& getDiffractometerType() const;
	//! Gets a shared pointer to the diffractometer related to the experiment
	std::shared_ptr<Diffractometer> getDiffractometer() const;
	//! Get a reference to the data
	const std::map<std::string,IData*>& getData() const;
	//! Gets the pointer to a given data stored in the experiment
	IData* getData(const std::string& name);
	//! Gets the names of the data stored in the experiment
	std::vector<std::string> getDataNames() const;
	//! Gets the name of the experiment
	const std::string& getName() const;
	//! Sets the name of the experiment
	void setName(const std::string& name);

	// Other methods

	//! Add some data to the experiment
	void addData(IData* data);
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
	std::shared_ptr<Diffractometer> _diffractometer;
	//! A map of the data related to the experiment. The keys are the basename of their corresponding file.
	std::map<std::string,IData*> _data;
};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_EXPERIMENT_H_ */
