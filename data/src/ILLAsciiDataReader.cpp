#include <cmath>
#include <cstring>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/spirit/include/qi.hpp>

#include "ILLAsciiDataReader.h"
#include "ILLAsciiMetaReader.h"
#include "MetaData.h"

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
	qi::phrase_parse(begin, end, *qi::int_ >> qi::eoi, ascii::space, v);
}

void readDoublesFromChar(const char* begin, const char* end, std::vector<double>& v)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	qi::phrase_parse(begin, end, *qi::double_ >> qi::eoi, ascii::space, v);
}

ILLAsciiDataReader::ILLAsciiDataReader() : IDataReader(), _nframes(0),_datapoints(0),_nangles(0),_header_size(0),_skipchar(0),_datalength(0),_map(),_map_address(nullptr)
{
}

std::vector<int> ILLAsciiDataReader::getFrame(uint i) const
{
	assert(i<_nframes);

	// Determine the beginning of the data block
	std::size_t begin=_header_size+(i+1)*_skipchar+i*_datalength;

	// Create vector and try to reserve a memory block
	std::vector<int> v;
	v.reserve(_datapoints);
	readIntsFromChar(_map_address+begin,_map_address+begin+_datalength,v);
	return v;
}



void ILLAsciiDataReader::open(const std::string& filename)
{
	_filename=filename;
	if ( !boost::filesystem::exists(filename.c_str()))
		throw std::runtime_error("ILLAsciiDataReader, file:"+filename+" does not exist");
	try
	{
		boost::interprocess::file_mapping filemap(filename.c_str(), boost::interprocess::read_only);
		_map=boost::interprocess::mapped_region(filemap,boost::interprocess::read_only);
	}
	catch(...)
	{
		throw;
	}

    // 81 characters per line, at least 100 lines of header
	std::size_t block_size=100*81;
	// Beginning of the blockILLAsciiDataReader
	_map_address=reinterpret_cast<char*>(_map.get_address());

	char* buffer=new char[block_size];
	strncpy(buffer, _map_address, block_size);
	ILLAsciiMetaReader* metareader=ILLAsciiMetaReader::Instance();
	_metadata=metareader->read(buffer,_header_size);
	delete [] buffer;
	// Extract some variables from the metadata
	_nframes=_metadata->getKey<int>("npdone");
	_datapoints=_metadata->getKey<int>("nbdata");
	_nangles=_metadata->getKey<int>("nbang");
	// Skip 8 or 9 lines to the beginning of data blocks
	_skipchar=81*(8+(_nangles<=2 ? 0 : 1));
	// ILL Ascii file for 2D detector store 10 values per line.
	_datalength=static_cast<int>(std::ceil(_datapoints/10.0))*81;

	//
	int skip3lines =3*81;
	std::vector<int> vi;

	readIntsFromChar(_map_address+_header_size+skip3lines,_map_address+_header_size+skip3lines+16,vi);

	if ((int)(vi.size() != 2) && (vi[0] != (int)(_nangles-3)))
		throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock1.");

	const char* beginvalues=_map_address+_header_size+skip3lines +(vi[1]+1)*81;

	std::vector<double> vd;
	int numberoflinestoread=vi[0]/5+1;
	readDoublesFromChar(beginvalues,beginvalues+numberoflinestoread*81,vd);

	if (vd.size() != _nangles+3)
		throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock2.");

	_metadata->add<double>("monitor",vd[1]);
}

uint ILLAsciiDataReader::nFrames() const
{
	return _nframes;
}

ILLAsciiDataReader::~ILLAsciiDataReader()
{

}

} // end namespace Data

} // end namespace SX
