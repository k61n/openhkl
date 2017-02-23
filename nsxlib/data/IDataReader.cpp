/*
 * IDataReader.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: pellegrini
 */

#include <cassert>

#include "../data/IDataReader.h"

namespace SX {

namespace Data {

IDataReader::IDataReader(const std::string& filename, const SX::Instrument::Diffractometer& diffractometer)
: _diffractometer(diffractometer),
  _nFrames(0),
  _nRows(0),
  _nCols(0),
  _states(),
  _fileSize(0),
  _isOpened(false),
  _inMemory(false),
  _isCached(true)
{
    _metadata.add<std::string>("filename",filename);
}

size_t IDataReader::getNFrames() const {
    return _nFrames;
}

size_t IDataReader::getNCols() const {
    return _nCols;
}

size_t IDataReader::getNRows() const {
    return _nRows;
}

const MetaData& IDataReader::getMetadata() const {
    return _metadata;
}

const Diffractometer& IDataReader::getDiffractometer() const {
    return _diffractometer;
}

InstrumentState IDataReader::getState(size_t frame) const
{
    assert(frame < _Frames);

    return _states[frame];
}

} // end namespace Data

} // end namespace SX
