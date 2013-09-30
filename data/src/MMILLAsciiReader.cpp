#include "MMILLAsciiReader.h"
#include "ILLAsciiMetaReader.h"
#include "MetaData.h"
#include <boost/filesystem.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include <cstring>

namespace SX
{

MMILLAsciiReader::MMILLAsciiReader(const std::string& filename)
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
	MetaData* m=metareader->read(buffer);
	delete [] buffer;
	return m;
}


} // end namespace SX
