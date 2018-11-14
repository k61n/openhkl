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
#include <sstream>
#include <string>
#include <vector>

#include <boost/interprocess/mapped_region.hpp>

#include <Eigen/Dense>

#include "IDataReader.h"
#include "InstrumentTypes.h"

namespace nsx {

//! ILL ascii data format
class ILLDataReader: public IDataReader {

public:

    static IDataReader* create(const std::string& filename, Diffractometer *diffractometer);

    //! Default constructor
    ILLDataReader(const std::string& filename, Diffractometer *diffractometer);

    //! Copy constructor
    ILLDataReader(const ILLDataReader& other)=delete;

    //! Destructor
    virtual ~ILLDataReader()=default;

    //! Assignment operator
    ILLDataReader& operator=(const ILLDataReader& other)=delete;

    // Other methods
    void open() override;

    void close() override;

    //! Read a single frame
    Eigen::MatrixXi data(size_t frame) override;

private:

    static std::size_t BlockSize;

    //! Invoke seekg to beginning the line number, at position pos. First line is 1
    void goToLine(std::stringstream& buffer, int number,int pos);
    //! Read the control block containing all float parameters.
    void readControlFBlock(std::stringstream&);
    //! Read the control block containing all Integer Metadata.
    void readControlIBlock(std::stringstream&);
    //! Read the file header containing the numor, user, instr, local contact, date and time
    //! This is all fixed format.
    void readHeader(std::stringstream&);
    //! Reads MetaData from a chain of characters as written in legacy ILL format
    //! return a MetaData Object
    void readMetadata(const char* buf);

    std::size_t _dataPoints;
    std::size_t _nAngles;
    std::size_t _headerSize;
    std::size_t _skipChar;
    std::size_t _dataLength;
    boost::interprocess::mapped_region _map;
    const char* _mapAddress;
    std::size_t _currentLine;
};

} // end namespace nsx
