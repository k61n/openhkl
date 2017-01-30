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

#ifndef NSXTOOL_ILLASCIIDATA_H_
#define NSXTOOL_ILLASCIIDATA_H_

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/interprocess/mapped_region.hpp>

#include <Eigen/Dense>

#include <IData.h>
#include "Diffractometer.h"


namespace SX {
    namespace Utils {
        template <class T> struct IMatrixParser;
    }
}

namespace SX
{

namespace Data
{
/*! \brief Legacy ILL Data in ASCII format.
 *
 */
class ILLAsciiData final: public IData
{
public:

    static IData* create(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer);

    //! Default constructor
    ILLAsciiData(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer);
    //! Copy constructor
    ILLAsciiData(const ILLAsciiData& other)=delete;
    //! Destructor
    virtual ~ILLAsciiData();

    // Operators

    //! Assignment operator
    ILLAsciiData& operator=(const ILLAsciiData& other)=delete;

    // Other methods
    void open() override;
    void close() override;
    //! Read a single frame
    Eigen::MatrixXi readFrame(std::size_t idx) override;

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
    void readMetaData(const char* buf);

    std::size_t _dataPoints;
    std::size_t _nAngles;
    std::size_t _headerSize;
    std::size_t _skipChar;
    std::size_t _dataLength;
    boost::interprocess::mapped_region _map;
    const char* _mapAddress;
    std::size_t _currentLine;
    SX::Utils::IMatrixParser<const char*>* _parser;


};

} // end namespace Data

} // end namespace SX

#endif /* NSXTOOL_ILLASCIIDATA_H_ */
