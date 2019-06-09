//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/ILLDataReader.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_DATA_ILLDATAREADER_H
#define CORE_DATA_ILLDATAREADER_H

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/interprocess/mapped_region.hpp>

#include <Eigen/Dense>

#include "core/loader/IDataReader.h"

namespace nsx {

//! ILL ascii data format
class ILLDataReader : public IDataReader {

public:
    ILLDataReader() = delete;
    ILLDataReader(const ILLDataReader& other) = delete;
    ILLDataReader(const std::string& filename, Diffractometer* diffractometer);
    ~ILLDataReader() = default;
    ILLDataReader& operator=(const ILLDataReader& other) = delete;

    // Other methods
    void open() final;

    void close() final;

    //! Read a single frame
    Eigen::MatrixXi data(size_t frame) final;

private:
    static std::size_t BlockSize;

    //! Invoke seekg to beginning the line number, at position pos. First line is
    //! 1
    void goToLine(std::stringstream& buffer, int number, int pos);

    //! Read the control block containing all float parameters.
    void readControlFBlock(std::stringstream&);

    //! Read the control block containing all Integer Metadata.
    void readControlIBlock(std::stringstream&);

    //! Read the file header containing the numor, user, instr, local contact,
    //! date and time This is all fixed format.
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

#endif // CORE_DATA_ILLDATAREADER_H
