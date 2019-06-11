//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/ILLDataReader.cpp
//! @brief     Implements class ILLDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>

#include <boost/interprocess/file_mapping.hpp>

#include "core/detector/Detector.h"
#include "core/gonio/Component.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/loader/ILLDataReader.h"
#include "core/detector/MatrixParser.h"
#include "base/parser/Parser.h"
#include "base/utils/StringIO.h"
#include "base/utils/Units.h"

namespace nsx {

// 81 characters per line, at least 100 lines of header
std::size_t ILLDataReader::BlockSize = 100 * 81;

ILLDataReader::ILLDataReader(const std::string& filename, Diffractometer* diffractometer)
    : IDataReader(filename, diffractometer)
{
    try {
        boost::interprocess::file_mapping filemap(filename.c_str(), boost::interprocess::read_only);
        _map = boost::interprocess::mapped_region(filemap, boost::interprocess::read_only);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("ILLDataReader() caught exception: ") + e.what());
    }

    // Gets the actual file size
    std::ifstream filein(filename, std::ifstream::ate);
    size_t actual_file_size = filein.tellg();
    filein.close();

    // Beginning of the blockILLAsciiDataReader
    _mapAddress = reinterpret_cast<char*>(_map.get_address());
    std::vector<char> buffer(BlockSize + 1,
                             0); // +1 to make space for 0 at end of string
    strncpy(&buffer[0], _mapAddress, BlockSize);
    readMetadata(&buffer[0]);

    // Extract some variables from the metadata
    _nFrames = size_t(_metadata.key<int>("npdone"));

    _dataPoints = size_t(_metadata.key<int>("nbdata"));
    _nAngles = size_t(_metadata.key<int>("nbang"));

    // Skip 8 or 9 lines to the beginning of data blocks
    _skipChar = 81 * (8 + (_nAngles <= 2 ? 0 : 2));

    // ILL Ascii file for 2D detector store 10 values per line.
    _dataLength = size_t(std::lround(std::ceil(_dataPoints / 10.0)) * 81);

    size_t predicted_file_size = _headerSize + _nFrames * (_skipChar + _dataLength);

    if (predicted_file_size != actual_file_size)
        throw std::runtime_error("Error when reading " + filename + ". The file is corrupted.");

    // Gets the value of the monitor for the first frame
    std::vector<int> vi;
    std::size_t skip3Lines = 3 * 81;
    readIntsFromChar(
        _mapAddress + _headerSize + skip3Lines, _mapAddress + _headerSize + skip3Lines + 16, vi);
    if ((vi.size() != 2) || (vi[0] != int(_nAngles + 3))) {
        throw std::runtime_error("Problem parsing numor: mismatch between number "
                                 "of angles in header and datablock 1.");
    }

    // This corresponds to the lines going from SSSSSSSSS to "            time
    // monitor       Total Cou     angles*1000"
    std::size_t fromStoFData = skip3Lines + size_t(vi[1] + 1) * 81;

    const char* beginValues = _mapAddress + _headerSize + fromStoFData;
    std::vector<double> vd;
    std::size_t FData = 81 * size_t(std::lround(std::ceil(vi[0] / 6.0)));
    readDoublesFromChar(beginValues, beginValues + FData, vd);

    if (vd.size() != (_nAngles + 3)) {
        throw std::runtime_error("Problem parsing numor: mismatch between number "
                                 "of angles in header and datablock 2.");
    }
    _metadata.add<double>("time", vd[0]);
    _metadata.add<double>("monitor", vd[1]);

    // This vector will store the id of the axis scanned by MAD. The id of the ith
    // scanned axis is defined with "icdesci" in the IIIIIIIII metadata block
    std::vector<unsigned int> scannedAxisId;
    scannedAxisId.reserve(_nAngles);

    for (std::size_t i = 0; i < _nAngles; ++i) {
        std::string idesc = std::string("icdesc") + std::to_string(i + 1);
        unsigned int id = static_cast<unsigned int>(_metadata.key<int>(idesc));
        scannedAxisId.push_back(id);
    }

    // This map relates the ids of the physical axis registered in the instrument
    // definition file with their name
    std::map<size_t, std::string> instrument_axis;
    const auto* detector = _diffractometer->detector();
    if (detector) {
        const auto& detector_gonio = _diffractometer->detector()->gonio();
        size_t n_detector_gonio_axes = detector_gonio.nAxes();
        ;
        for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
            const auto& axis = detector_gonio.axis(i);
            instrument_axis.insert(std::make_pair(axis.id(), axis.name()));
        }
    }

    const auto& sample_gonio = _diffractometer->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();
    ;
    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const auto& axis = sample_gonio.axis(i);
        instrument_axis.insert(std::make_pair(axis.id(), axis.name()));
    }

    const auto& source_gonio = _diffractometer->source().gonio();
    size_t n_source_gonio_axes = source_gonio.nAxes();
    ;
    for (size_t i = 0; i < n_source_gonio_axes; ++i) {
        const auto& axis = source_gonio.axis(i);
        instrument_axis.insert(std::make_pair(axis.id(), axis.name()));
    }

    // Check that every scanned axis has been defined in the instrument file.
    // Otherwise, throws.
    for (const auto& id : scannedAxisId) {
        auto it = instrument_axis.find(id);
        if (it == instrument_axis.end()) {
            throw std::runtime_error(
                "The axis with id " + std::to_string(id)
                + " is not defined in instrument configuration file.");
        }
    }

    // This map will store the values over the frames of each physical axis of the
    // goniometers bound to the instrument (detector + sample + source) 3 cases:
    //	1) a physical axis is not a scanned axis and its name is not defined in
    // the metadata, then the corresponding values will 	   be a constant equal
    // to 0 	2) a physical axis is not a scanned axis and its name is defined
    // in the metadata, then the corresponding values will 	   be a constant
    // equal to the metadata value
    // 	3) a physical axis is one of the scanned axis, then the corresponding
    // values will be fetched from the FFFFFF dta blocks
    std::map<unsigned int, std::vector<double>> gonioValues;

    // Loop over the physical axis of the instrument
    for (auto&& p : instrument_axis) {
        // Case of a physical axis that is also a scanned axis
        // Prepare a vector that will be further set with the corresponding values
        auto it = std::find(scannedAxisId.begin(), scannedAxisId.end(), p.first);

        if (it != scannedAxisId.end()) {
            std::vector<double> values;
            values.reserve(_nFrames);
            gonioValues.insert(std::pair<unsigned int, std::vector<double>>(p.first, values));
        }
        // Case of a physical axis that is not a scanned axis.
        // The values for this axis will be a constant
        else {
            // If the axis name is defined in the metadata, the constant will be the
            // corresponding value other wise it will be 0
            double val = _metadata.isKey(p.second) ? _metadata.key<double>(p.second) : 0.0;
            // Data are given in deg for angles
            std::vector<double> values(_nFrames, val * deg);
            gonioValues.insert(std::pair<unsigned int, std::vector<double>>(p.first, values));
        }
    }

    // This is the address of the beginning of the first SSSSSS datablock
    const char* start = _mapAddress + _headerSize;
    // Read the FFFFFF data block frame by frame
    for (unsigned int i = 0; i < _nFrames; ++i) {
        std::vector<double> scannedValues;
        beginValues = start + i * (_dataLength + _skipChar) + fromStoFData;
        readDoublesFromChar(beginValues, beginValues + FData, scannedValues);

        // The values of the scanned axis starts from the 4th number of the FFFFFF
        // data block. The first three being set for the 'time', the 'monitor' and
        // the 'total count' values.
        unsigned int comp(3);
        for (const auto& id : scannedAxisId) {
            // The values are given in degrees in the metadata block and multiplied by
            // 1000
            gonioValues[id].push_back(scannedValues[comp++] / 1000 * deg);
        }
    }

    _sampleStates.resize(_nFrames);
    _detectorStates.resize(_nFrames);

    // If a detector is set for this instrument, loop over the frames and gather
    // for each physical axis of the detector the corresponding values defined
    // previously. The gathered values being further pushed as a new detector
    // state
    if (detector) {
        const auto& detector_gonio = _diffractometer->detector()->gonio();
        size_t n_detector_gonio_axes = detector_gonio.nAxes();
        ;
        for (size_t i = 0; i < _nFrames; ++i) {
            std::vector<double> detValues;
            detValues.reserve(n_detector_gonio_axes);
            for (size_t j = 0; j < n_detector_gonio_axes; ++j) {
                const auto& axis = detector_gonio.axis(j);
                detValues.push_back(gonioValues[axis.id()][i]);
            }
            _detectorStates[i] = detValues;
        }
    }

    // Loop over the frames and gather for each physical axis
    // of the sample the corresponding values defined previously. The gathered
    // values being further pushed as a new sample state
    for (size_t i = 0; i < _nFrames; ++i) {
        std::vector<double> sampleValues;
        sampleValues.reserve(n_sample_gonio_axes);
        for (size_t j = 0; j < n_sample_gonio_axes; ++j) {
            const auto& axis = sample_gonio.axis(j);
            sampleValues.push_back(gonioValues[axis.id()][i]);
        }
        _sampleStates[i] = sampleValues;
    }

    _fileSize = _map.get_size();

    close();
}

void ILLDataReader::open()
{
    if (_isOpened)
        return;
    try {
        boost::interprocess::file_mapping filemap(
            _metadata.key<std::string>("filename").c_str(), boost::interprocess::read_only);
        boost::interprocess::mapped_region reg(filemap, boost::interprocess::read_only);
        _map.swap(reg);
        _mapAddress = reinterpret_cast<char*>(_map.get_address());
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("ILLAsciiData::open() caught exception: ") + e.what());
    }
    _isOpened = true;
}

void ILLDataReader::close()
{
    if (!_isOpened)
        return;
    _map = boost::move(boost::interprocess::mapped_region());
    _isOpened = false;
}

Eigen::MatrixXi ILLDataReader::data(size_t frame)
{
    assert(frame < _nFrames);

    if (!_isOpened)
        open();

    // Determine the beginning of the data block
    std::size_t begin = _headerSize + (frame + 1) * _skipChar + frame * _dataLength;
    // Create vector and try to reserve a memory block
    Eigen::MatrixXi v;
    v.resize(long(_nRows), long(_nCols));
    assert(_nRows >= 1);
    assert(_nCols >= 1);

    MatrixParser parser;
    parser(_diffractometer->detector()->dataOrder(), _mapAddress + begin, _dataLength, v);

    return v;
}

void ILLDataReader::goToLine(std::stringstream& buffer, int number, int pos)
{
    int char_number = 81 * (number - 1) + pos;
    buffer.seekg(char_number, std::ios::beg);
}

void ILLDataReader::readControlIBlock(std::stringstream& buffer)
{
    // Read the I block, starting at line 12
    goToLine(buffer, 12, 0);

    // Read the number of parameters to be read, and number of lines
    size_t tot, lines;
    buffer >> tot >> lines;

    // Create vector for entries
    std::vector<std::string> Ientries(tot);
    goToLine(buffer, 13, 0);

    // Read the metadata keys
    for (size_t i = 0; i < tot; ++i)
        buffer >> Ientries[i];

    // Goto the line containing values
    goToLine(buffer, int(13 + lines), 0);
    int value;
    for (size_t i = 0; i < tot; ++i) {
        buffer >> value;
        // Skip any 0 values in this block
        if (value != 0)
            _metadata.add<int>(Ientries[i], value);
    }
    _currentLine = 13 + 2 * lines;
}

void ILLDataReader::readControlFBlock(std::stringstream& buffer)
{
    _currentLine++; // Skip the FFFFFFF line.
    goToLine(buffer, int(_currentLine), 0);
    // Total number of entries and number of lines
    int nTot, nLines;
    buffer >> nTot >> nLines;
    // Size of each block
    int sizeBlock = 16;
    // Number of full lines
    int fullLines = nTot / 5;
    // Number of elements in the non full line
    int missing = nTot - 5 * nLines;
    // Unfortunately this is a fixed format, and there are spaces from some keys
    // need to read.
    std::vector<char> line(81, 0); // 81 to make space for null character at string end
    std::string s1, s2;
    std::vector<std::string> keys(static_cast<size_t>(nTot));
    int counter = 0;

    // read the keys in temp vector
    for (int i = 0; i < fullLines; ++i) {
        goToLine(buffer, int(++_currentLine), 0);
        buffer.read(&line[0], 80);
        s1 = &line[0];

        for (int j = 0; j < 5; j++) {
            s2 = clear_spaces(s1.substr(size_t(j * sizeBlock), size_t(sizeBlock)));
            keys[size_t(counter++)] = s2;
        }
    }

    // If non complete line.
    if (missing) {
        goToLine(buffer, int(++_currentLine), 0);
        buffer.read(&line[0], 80);
        s1 = &line[0];
        for (int j = 0; j < missing; j++) {
            s2 = clear_spaces(s1.substr(size_t(j * sizeBlock), size_t(sizeBlock)));
            keys[size_t(counter++)] = s2;
        }
    }

    double value;
    // Read the values
    goToLine(buffer, int(++_currentLine), 0);
    for (size_t i = 0; i < size_t(nTot); ++i) {
        buffer >> value;
        // Ignore spare member blocks.
        if (keys[i].compare("(spare)"))
            _metadata.add<double>(keys[i], value);
    }
    _currentLine += size_t(fullLines + missing);
}

void ILLDataReader::readHeader(std::stringstream& buffer)
{
    // _buffer to contains a full line
    std::vector<char> line(81, 0);
    // Two temp strings
    std::string s1, s2;
    // Go to Line 2, grab the numor
    goToLine(buffer, 2, 0);
    int number;
    buffer >> number;
    _metadata.add<int>("Numor", number);

    // Go to Line 6, grab instrument, User, Local contact, date and Time.
    goToLine(buffer, 6, 0);
    buffer.read(&line[0], 80);
    s1 = &line[0];
    s2 = trim(s1.substr(0, 4));
    _metadata.add<std::string>("Instrument", s2);
    s2 = trim(s1.substr(4, 6));
    _metadata.add<std::string>("User", s2);
    s2 = trim(s1.substr(10, 4));
    _metadata.add<std::string>("LocalContact", s2);
    s2 = s1.substr(14, 9);
    _metadata.add<std::string>("Date", s2);
    s2 = s1.substr(24, 8);
    _metadata.add<std::string>("Time", s2);
    // Go to Line 10, grab the title and type of scan
    goToLine(buffer, 10, 0);
    buffer.read(&line[0], 80);
    s1 = &line[0];
    s2 = trim(s1.substr(0, 72));
    _metadata.add<std::string>("Title", s2);
    s2 = trim(s1.substr(72, 8));
    _metadata.add<std::string>("ScanType", s2);
    std::string date, time;

    // Enter a key for the posix time
    date = _metadata.key<std::string>("Date");
    time = _metadata.key<std::string>("Time");
    _metadata.add<std::string>("date", date);
    _metadata.add<std::string>("time", time);
}

void ILLDataReader::readMetadata(const char* buf)
{
    std::string s(buf);
    std::size_t endMetadata = s.find("SSSSSSSS");

    if (endMetadata == std::string::npos)
        throw std::runtime_error("Could not find end of metadata block");

    // Number of characters up to the "SSSS...." line
    _headerSize = endMetadata;
    _currentLine = 0;

    std::stringstream buffer;
    buffer << s.substr(0, endMetadata);
    // Try to read the header file
    try {
        readHeader(buffer);
    } catch (std::exception& e) {
        throw std::runtime_error(
            std::string("ILLAsciiData: failed to read header in stream: ") + e.what());
    } catch (...) {
        throw std::runtime_error("ILLAsciiData: Fail to read Header in stream (unknown exception)");
    }

    // Read the block containing control parameters (integer)
    try {
        readControlIBlock(buffer);
    } catch (...) {
        throw std::runtime_error("ILLAsciiMetaReader: Fail to read IBlock in stream");
    }

    // Read the block containing float metadata
    try {
        readControlFBlock(buffer);
    } catch (...) {
        throw std::runtime_error("ILLAsciiMetaReader: Fail to read FBlock in stream");
    }
}

} // namespace nsx
