/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
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

#ifndef NSXTOOL_IDATA_H_
#define NSXTOOL_IDATA_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ComponentState.h"
#include "MetaData.h"
#include "Diffractometer.h"

namespace SX
{

namespace Instrument
{
	class Component;
}

namespace Data
{

using namespace SX::Instrument;

class IData
{
public:

	typedef std::vector<int> vint;

	// Constructors and destructor

	//! Constructor
	IData(const std::string& filename, std::shared_ptr<Diffractometer> instrument, bool inMemory=false);
	//! Copy constructor
	IData(const IData& other)=delete;
	//! Destructor
	virtual ~IData()=0;

	// Operators

	//! Assignment operator
	IData& operator=(const IData& other)=delete;

	// Getters and setters

    //! Interpolate between two consecutive detector states
    ComponentState getDetectorInterpolatedState(double frame);
	const std::string& getFilename() const;
	std::shared_ptr<Diffractometer> getDiffractometer() const;
	MetaData* const getMetadata() const;
    //! Interpolate between two consecutive sample states
    ComponentState getSampleInterpolatedState(double frame);

	// Other methods

	//! Return true if the file is stored in memory
	bool isInMemory() const;
	//! Return the intensity at point x,y,z.
	virtual int dataAt(int x=0, int y=0, int z=0)=0;
    //! Read a given Frame of the data
    virtual std::vector<int> getFrame(std::size_t i)=0;
    //! Read all the frames in memory
    virtual void readInMemory()=0;
    // Release the data from memory
    virtual void releaseMemory()=0;

protected:

	std::string _filename;
	std::shared_ptr<Diffractometer> _diffractometer;
	MetaData* _metadata;
	bool _inMemory;
	std::vector<vint> _data;
	std::vector<ComponentState> _detectorStates;
	std::vector<ComponentState> _sampleStates;

};

} // end namespace Data

} // end namespace SX


#endif // NSXTOOL_IDATA_H_
