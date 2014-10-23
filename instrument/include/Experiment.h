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
	Experiment(const std::string& name, std::shared_ptr<Diffractometer> diffractometer);
	//! Destructor
	virtual ~Experiment();

	// Operators

	//! Assignment operator
	Experiment& operator=(const Experiment& other);

	// Getters and setters

	//! Gets a shared pointer to the diffractometer related to the experiment
	std::shared_ptr<Diffractometer> getDiffractometer() const;
	//! Gets the names of the data stored in the experiment
	std::vector<std::string> getDataNames() const;
	//! Gets the name of the experiment
	std::string getName() const;

	// Other methods

	//! Add some data to the experiment
	void addData(IData* data);

private:
	//! The name of this experiment
	std::string _name;
	//! A shared pointer to the detector assigned to this experiment
	std::shared_ptr<Diffractometer> _diffractometer;
	std::map<std::string,IData*> _data;
};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_EXPERIMENT_H_ */
