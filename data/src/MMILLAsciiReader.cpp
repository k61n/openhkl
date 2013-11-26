#include "MMILLAsciiReader.h"
#include "ILLAsciiMetaReader.h"
#include "MetaData.h"
#include <boost/filesystem.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include <cstring>
#include <boost/spirit/include/qi.hpp>
#include <cmath>

namespace SX
{

namespace Data
{

// Method to read a vector of int values from two char pointers, using spirit
// This is faster than the C atoi() function.
void readIntsFromChar(const char* begin, const char* end, std::vector<int>& v)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	qi::phrase_parse(begin, end,
        *qi::int_ >> qi::eoi, ascii::space, v);
}

MMILLAsciiReader::MMILLAsciiReader(const std::string& filename):
		_isInitialized(false),_nframes(0),_datapoints(0),_nangles(0),_header_size(0),_skipchar(0),_datalength(0)
{
	if ( !boost::filesystem::exists(filename.c_str()))
		throw std::runtime_error("MMILLAsciiReader, file:"+filename+" does not exist");
	try
	{
	_map=boost::interprocess::file_mapping(filename.c_str(), boost::interprocess::read_only);
	}
	catch(...)
	{
		throw;
	}


}
MMILLAsciiReader::~MMILLAsciiReader()
{

}

MetaData* MMILLAsciiReader::readMetaDataBlock(int nlines)
{
	//81 characters per line
	std::size_t block_size=nlines*81;
	//Map the region corresponding to the metadata block
	boost::interprocess::mapped_region mdblock(_map,boost::interprocess::read_only,0,block_size);
	//Beginning of the block
	const char* b=reinterpret_cast<char*>(mdblock.get_address());
	char* buffer=new char[block_size];
	strncpy(buffer, b, block_size);
	ILLAsciiMetaReader* metareader=ILLAsciiMetaReader::Instance();
	MetaData* m=metareader->read(buffer,_header_size);
	delete [] buffer;
	//
	_nframes=m->getKey<int>("npdone");
	_datapoints=m->getKey<int>("nbdata");
	_nangles=m->getKey<int>("nbang");
	// Skip 8 or 9 lines to the beginning of data blocks
	_skipchar=81*(8+(_nangles<=2 ? 0 : 1));
	// ILL Ascii file for 2D detector store 10 values per line.
	_datalength=static_cast<int>(std::ceil(_datapoints/10.0))*81;
	_isInitialized=true;
	return m;
}

std::vector<int> MMILLAsciiReader::readBlock(unsigned int i) const
{
	if (!_isInitialized)
		throw std::runtime_error("MMILLAsciireader: memory mapped filed is not initialized");
	if (i>_nframes-1)
		throw std::runtime_error("MMILLAsciiReader:readBlock, frame index not valid");

	// Determine the beginning of the data block
	std::size_t begin=_header_size+(i+1)*_skipchar+i*_datalength;
	// Map the region of interest in the file
	boost::interprocess::mapped_region mdblock(_map,boost::interprocess::read_only,begin,_datalength);
	const char* b=reinterpret_cast<char*>(mdblock.get_address());
	// Create vector and try to reseve a memory block
	std::vector<int> v;
	try
	{
		v.reserve(_datapoints);
	}catch(...)
	{
		throw std::runtime_error("MMILLAsciiReader: problem reserving size of vector");
	}
	readIntsFromChar(b,b+_datalength,v);
	if (v.size()!=_datapoints)
		throw std::runtime_error("MMILLAsciiReader::readBlock, number of data points read different from expected");
	return v;
}

} // end namespace Data

} // end namespace SX
