#include <cmath>
#include <cstring>
#include <map>
#include <set>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include "../data/ILLDataReader.h"
#include "../instrument/Component.h"
#include "../instrument/ComponentState.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Gonio.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../utils/MatrixParser.h"
#include "../utils/Parser.h"
#include "../utils/Units.h"

using SX::Utils::readIntsFromChar;
using SX::Utils::readDoublesFromChar;
using SX::Instrument::ComponentState;

namespace SX {

namespace Data {

using SX::Instrument::Detector;

// 81 characters per line, at least 100 lines of header
std::size_t ILLDataReader::BlockSize = 100*81;

IDataReader* ILLDataReader::create(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer)
{
    return new ILLDataReader(filename, diffractometer);
}

ILLDataReader::ILLDataReader(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer)
: IDataReader(filename,diffractometer)
{
    try {
        boost::interprocess::file_mapping filemap(filename.c_str(), boost::interprocess::read_only);
        _map = boost::interprocess::mapped_region(filemap, boost::interprocess::read_only);
    }
    catch(std::exception& e) {
        throw std::runtime_error(std::string("ILLDataReader() caught exception: ") + e.what());
    }

    // Beginning of the blockILLAsciiDataReader
    _mapAddress = reinterpret_cast<char*>(_map.get_address());
    std::vector<char> buffer(BlockSize+1, 0); // +1 to make space for 0 at end of string
    strncpy(&buffer[0], _mapAddress, BlockSize);
    readMetadata(&buffer[0]);

    // Extract some variables from the metadata
    _nFrames = size_t(_metadata.getKey<int>("npdone"));
    _dataPoints = size_t(_metadata.getKey<int>("nbdata"));
    _nAngles = size_t(_metadata.getKey<int>("nbang"));

    // Skip 8 or 9 lines to the beginning of data blocks
    _skipChar = 81*(8+(_nAngles<=2 ? 0 : 2));

    // ILL Ascii file for 2D detector store 10 values per line.
    _dataLength = size_t(std::lround(std::ceil(_dataPoints/10.0))*81);

    // Get the value of the monitor for the first frame
    std::vector<int> vi;
    std::size_t skip3Lines=3*81;
    readIntsFromChar(_mapAddress+_headerSize+skip3Lines,_mapAddress+_headerSize+skip3Lines+16,vi);
    if ((vi.size() != 2) || (vi[0] != int(_nAngles+3))) {
        throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock 1.");
    }

    // This corresponds to the lines going from SSSSSSSSS to "            time         monitor       Total Cou     angles*1000"
    std::size_t fromStoFData = skip3Lines + size_t(vi[1]+1)*81;

    const char* beginValues = _mapAddress + _headerSize + fromStoFData;
    std::vector<double> vd;
    std::size_t FData = 81*size_t(std::lround(std::ceil(vi[0]/6.0)));
    readDoublesFromChar(beginValues, beginValues + FData, vd);

    if (vd.size() != (_nAngles+3)) {
        throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock 2.");
    }
    _metadata.add<double>("time", vd[0]);
    _metadata.add<double>("monitor", vd[1]);

    // This vector will store the id of the axis scanned by MAD. The id of the ith scanned axis is defined with "icdesci"
    // in the IIIIIIIII metadata block
    std::vector<unsigned int> scannedAxisId;
    scannedAxisId.reserve(_nAngles);

    for (std::size_t i = 0; i < _nAngles; ++i) {
        std::string idesc = std::string("icdesc") + std::to_string(i+1);
        unsigned int id = static_cast<unsigned int>(_metadata.getKey<int>(idesc));
        scannedAxisId.push_back(id);
    }

    // This map relates the ids of the physical axis registered in the instrument definition file with their name
    std::map<unsigned int,std::string> instrPhysAxisIds(_diffractometer->getPhysicalAxesNames());

    // Check that every scanned axis has been defined in the instrumnet file. Otherwise, throws.
    for (const auto& id: scannedAxisId) {
        auto it = instrPhysAxisIds.find(id);
        if (it == instrPhysAxisIds.end())
            throw std::runtime_error(
                    "The axis with MAD id " + std::to_string(id) +
                    " could not be found in the instrument definition file.");
    }

    // This map will store the values over the framess of each physical axis of the goniometers bound to the instrument (detector + sample + source)
    // 3 cases:
    //	1) a physical axis is not a scanned axis and its name is not defined in the metadata, then the corresponding values will
    //	   be a constant equal to 0
    //	2) a physical axis is not a scanned axis and its name is defined in the metadata, then the corresponding values will
    //	   be a constant equal to the metadata value
    // 	3) a physical axis is one of the scanned axis, then the corresponding values will be fetched from the FFFFFF dta blocks
    std::map<unsigned int,std::vector<double>> gonioValues;

    // Loop over the physical axis of the instrument
    for (const auto& p : instrPhysAxisIds) {
        // Case of a physical axis that is also a scanned axis
        // Prepare a vector that will be further set with the corresponding values
        auto it = std::find(scannedAxisId.begin(), scannedAxisId.end(), p.first);

        if (it != scannedAxisId.end()) {
            std::vector<double> values;
            values.reserve(_nFrames);
            gonioValues.insert(std::pair<unsigned int,std::vector<double>>(p.first, values));
        }
        // Case of a physical axis that is not a scanned axis.
        // The values for this axis will be a constant
        else {
            // If the axis name is defined in the metadata, the contant will be the corresponding value
            // other wise it will be 0
            double val = _metadata.isKey(p.second) ? _metadata.getKey<double>(p.second) : 0.0;
            // Data are given in deg for angles
            std::vector<double> values(_nFrames,val*SX::Units::deg);
            gonioValues.insert(std::pair<unsigned int,std::vector<double>>(p.first,values));
        }
    }

    // This is the address of the beginning of the first SSSSSS datablock
    const char* start = _mapAddress+_headerSize;
    // Read the FFFFFF data block frame by frame
    for (unsigned int i = 0; i < _nFrames; ++i) {
        std::vector<double> scannedValues;
        beginValues = start  + i*(_dataLength+_skipChar) + fromStoFData;
        readDoublesFromChar(beginValues,beginValues+FData,scannedValues);

        // The values of the scanned axis starts from the 4th number of the FFFFFF data block.
        // The first three being set for the 'time', the 'monitor' and the 'total count' values.
        unsigned int comp(3);
        for (const auto& id: scannedAxisId) {
            // The values are given in degrees in the metadata block and multiplied by 1000
            gonioValues[id].push_back(scannedValues[comp++]/1000*SX::Units::deg);
        }
    }

    _states.resize(_nFrames);
    //_detectorStates.reserve(_nFrames);
    auto detector = _diffractometer->getDetector();
    // If a detector is set for this instrument, loop over the frames and gather for each physical axis
    // of the detector the corresponding values defined previously. The gathered values being further pushed as
    // a new detector state
    if (detector) {
        auto detAxisIdsToNames = detector->getGonio()->getPhysicalAxesIds();
        for (std::size_t f = 0; f < _nFrames; ++f) {
            std::vector<double> detValues;
            detValues.reserve(detAxisIdsToNames.size());
            for (const auto& v: detAxisIdsToNames) {
                detValues.push_back(gonioValues[v][f]);
            }
            _states[f].detector = ComponentState(_diffractometer->getDetector().get(), detValues);
            //_detectorStates.push_back(_diffractometer->getDetector()->createState(detValues));
        }
    }

    //_sampleStates.reserve(_nFrames);
    auto sample = _diffractometer->getSample();
    // If a sample is set for this instrument, loop over the frames and gather for each physical axis
    // of the sample the corresponding values defined previously. The gathered values being further pushed as
    // a new sample state
    if (sample) {
        auto sampleAxisIdsToNames = sample->getGonio()->getPhysicalAxesIds();
        for (std::size_t f = 0; f < _nFrames; ++f) {
            std::vector<double> sampleValues;
            sampleValues.reserve(sampleAxisIdsToNames.size());
            for (const auto& v: sampleAxisIdsToNames) {
                sampleValues.push_back(gonioValues[v][f]);
            }
            _states[f].sample = ComponentState(_diffractometer->getSample().get(), sampleValues);
            //_sampleStates.push_back(_diffractometer->getSample()->createState(sampleValues));
        }
    }

    //_sourceStates.reserve(_nFrames);
    auto source = _diffractometer->getSource();
    // If a source is set for this instrument, loop over the frames and gather for each physical axis
    // of the source the corresponding values defined previously. The gathered values being further pushed as
    // a new source state
    if (source) {
        auto sourceAxisIdsToNames = source->getGonio()->getPhysicalAxesIds();
        for (std::size_t f = 0; f < _nFrames; ++f) {
            std::vector<double> sourceValues;
            sourceValues.reserve(sourceAxisIdsToNames.size());
            for (const auto& v: sourceAxisIdsToNames) {
                sourceValues.push_back(gonioValues[v][f]);
            }
            _states[f].source = ComponentState( _diffractometer->getSource().get(), sourceValues);
            //_sourceStates.push_back(_diffractometer->getSource()->createState(sourceValues));
        }
    }

    _fileSize = _map.get_size();
    close();

    std::shared_ptr<Detector> d = _diffractometer->getDetector();

    _parser = SX::Utils::getMatrixParser(d->getDataOrder());
}

ILLDataReader::~ILLDataReader()
{
    delete _parser;
}

void ILLDataReader::open()
{
    if (_isOpened)
        return;
    try {
        boost::interprocess::file_mapping filemap(_metadata.getKey<std::string>("filename").c_str(), boost::interprocess::read_only);
        boost::interprocess::mapped_region reg(filemap,boost::interprocess::read_only);
        _map.swap(reg);
        _mapAddress=reinterpret_cast<char*>(_map.get_address());
    } catch(std::exception& e) {
        throw std::runtime_error(std::string("ILLAsciiData::open() caught exception: ") + e.what());
    }
    _isOpened = true;
}

void ILLDataReader::close()
{
    if (!_isOpened) {
        return;
    }
    _map = boost::move(boost::interprocess::mapped_region());
    _isOpened = false;
}

Eigen::MatrixXi ILLDataReader::getData(size_t frame)
{
    assert(frame < _nFrames);

    if (!_isOpened)
        open();

    // Determine the beginning of the data block
    std::size_t begin = _headerSize+(frame+1)*_skipChar+frame*_dataLength;
    // Create vector and try to reserve a memory block
    Eigen::MatrixXi v;
    v.resize(long(_nRows), long(_nCols));
    assert(_nRows >= 1);
    assert(_nCols >= 1);

    (*_parser)(_mapAddress+begin,_dataLength,v);

    return v;
}

void ILLDataReader::goToLine(std::stringstream& buffer, int number, int pos)
{
    int char_number = 81*(number-1)+pos;
    buffer.seekg(char_number,std::ios::beg);
}

void ILLDataReader::readControlIBlock(std::stringstream& buffer)
{
    // Read the I block, starting at line 12
    goToLine(buffer,12,0);

    // Read the number of parameters to be read, and number of lines
    size_t tot, lines;
    buffer >> tot >> lines;

    // Create vector for entries
    std::vector<std::string> Ientries(tot);
    goToLine(buffer, 13, 0);

    // Read the metadata keys
    for (size_t i = 0; i < tot; ++i) {
        buffer >> Ientries[i];
    }

    // Goto the line containing values
    goToLine(buffer, int(13+lines), 0);
    int value;
    for (size_t i = 0; i < tot; ++i) {
        buffer >> value;
        // Skip any 0 values in this block
        if (value != 0) {
            _metadata.add<int>(Ientries[i], value);
        }
    }
    _currentLine = 13+2*lines;
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
    int fullLines = nTot/5;
    // Number of elements in the non full line
    int missing = nTot-5*nLines;
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
            s2 = s1.substr(size_t(j*sizeBlock), size_t(sizeBlock));
            // Remove all white space from the string (For example H (HMin) and concatenate string
            boost::erase_all(s2," ");
            keys[size_t(counter++)] = s2;
        }
    }

    // If non complete line.
    if (missing) {
        goToLine(buffer, int(++_currentLine), 0);
        buffer.read(&line[0], 80);
        s1 = &line[0];
        for (int j = 0; j < missing; j++) {
            s2 = s1.substr(size_t(j*sizeBlock), size_t(sizeBlock));
            // Remove all white space
            boost::erase_all(s2," ");
            keys[size_t(counter++)] = s2;
        }
    }

    double value;
    // Read the values
    goToLine(buffer, int(++_currentLine), 0);
    for (size_t i = 0; i < size_t(nTot); ++i) {
        buffer >> value;
        // Ignore spare member blocks.
        if (keys[i].compare("(spare)")) {
            _metadata.add<double>(keys[i], value);
        }
    }
    _currentLine += size_t(fullLines+missing);
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
    s2 = s1.substr(0, 4);
    boost::trim(s2);
    _metadata.add<std::string>("Instrument", s2);
    s2 = s1.substr(4, 6);
    boost::trim(s2);
    _metadata.add<std::string>("User", s2);
    s2 = s1.substr(10, 4);
    boost::trim(s2);
    _metadata.add<std::string>("LocalContact", s2);
    s2 = s1.substr(14, 9);
    _metadata.add<std::string>("Date", s2);
    s2 = s1.substr(24, 8);
    _metadata.add<std::string>("Time", s2);
    // Go to Line 10, grab the title and type of scan
    goToLine(buffer, 10, 0);
    buffer.read(&line[0], 80);
    s1 = &line[0];
    s2 = s1.substr(0,72);
    boost::trim(s2);
    _metadata.add<std::string>("Title", s2);
    s2 = s1.substr(72,8);
    boost::trim(s2);
    _metadata.add<std::string>("ScanType", s2);
    std::string date, time;

    // Enter a key for the posix time
    date = _metadata.getKey<std::string>("Date");
    time = _metadata.getKey<std::string>("Time");
    // Now do the job.
    // In mad. the date is written in short UK notation (only 2 digits for the year) DD-MMM-YY
    // Need to convert to ISO format. Assume that no data prior to 2000 needs to be reanalysed.
    std::string full_date = date.substr(0,7)+std::string("20")+date.substr(7,2);
    // Add microsecs to the time to be compliant with boost
    std::string fulltime = time+std::string(".000");
    boost::posix_time::ptime pos_time(boost::gregorian::from_uk_string(full_date),boost::posix_time::duration_from_string(fulltime));
    _metadata.add<boost::posix_time::ptime>("ptime",pos_time);
}

void ILLDataReader::readMetadata(const char* buf)
{
    std::string s(buf);
    std::size_t endMetadata = s.find("SSSSSSSS");

    if (endMetadata == std::string::npos) {
        throw std::runtime_error("Could not find end of metadata block");
    }

    // Number of characters up to the "SSSS...." line
    _headerSize = endMetadata;
    _currentLine = 0;

    std::stringstream buffer;
    buffer << s.substr(0,endMetadata);
    // Try to read the header file
    try	{
        readHeader(buffer);
    }
    catch(std::exception& e) {
        throw std::runtime_error(std::string("ILLAsciiData: failed to read header in stream: ") + e.what());
    }
    catch(...) {
        throw std::runtime_error("ILLAsciiData: Fail to read Header in stream (unknown exception)");
    }

    // Read the block containing control parameters (integer)
    try	{
        readControlIBlock(buffer);
    }
    catch(...) {
        throw std::runtime_error("ILLAsciiMetaReader: Fail to read IBlock in stream");
    }

    // Read the block containing float metadata
    try {
        readControlFBlock(buffer);
    }
    catch(...) {
        throw std::runtime_error("ILLAsciiMetaReader: Fail to read FBlock in stream");
    }
}

} // end namespace Data

} // end namespace SX
