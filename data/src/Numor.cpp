#include <string>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Numor.h"

namespace SX {

namespace Data {

int Numor::_currentline=0;

Numor::Numor(const std::string& filename)
{
	_currentline=0;
	// Open file
	std::ifstream file;
	file.open(filename.c_str(),std::ios::in);
    std::stringstream _buffer;

    // Read only the header and parse the metadata. 200 lines should be enough.
    char* temp=new char[81*200];
    file.read(temp,81*200);
    std::string s(temp);
    std::size_t end_metadata=s.find("SSSSSSSS");

    if (end_metadata==std::string::npos)
    	throw std::runtime_error("Could not find end of metadata block in the first 200 lines of the ASCII file");

	// copy the entire metadata block to memory
	if (file.is_open())
	{
		_buffer << s.substr(0,end_metadata);
		file.close();
	}

	// Try to read the header file
	try
	{
		readHeader(_buffer);
	}catch(...)
	{
		throw std::runtime_error("Fail to read Header in file"+filename);
	}

	// Read the block containing control parameters (integer)
	try
	{
		readControlIBlock(_buffer);
	}catch(...)
	{
		throw std::runtime_error("Fail to read IBlock in file"+filename);
	}

	// Read the block containing float metadata
	try
	{
		readControlFBlock(_buffer);
	}catch(...)
	{
		throw std::runtime_error("Fail to read FBlock in file"+filename);
	}


}

Numor::Numor(const std::string& filename,const Numor& tocopy):_meta(tocopy._meta)
{
	_currentline=0;
	// Open file
	std::ifstream file;
	file.open(filename.c_str(),std::ios::in);
     std::stringstream _buffer;
	// copy the entire file to memory
	if (file.is_open())
	{
		_buffer << file.rdbuf();
		file.close();
	}

	// Try to read the header file
	try
	{
		readHeader(_buffer);
	}catch(...)
	{
		throw std::runtime_error("Fail to read Header in file"+filename);
	}

	// Read the block containing control parameters (integer)
	try
	{
		readControlIBlock(_buffer);
	}catch(...)
	{
		throw std::runtime_error("Fail to read IBlock in file"+filename);
	}

	// Read the block containing float metadata
	try
	{
		readControlFBlock(_buffer);
	}catch(...)
	{
		throw std::runtime_error("Fail to read FBlock in file"+filename);
	}

	 //read the data block
	try
	{
		readData(_buffer);
	}catch(...)
	{
		throw std::runtime_error("Fail to read Data block in file"+filename);
	}

}
Numor::Numor(const Numor& rhs)
{
	_number=rhs._number;
	_meta=rhs._meta;
	_rawdata.resize(rhs._rawdata.size());
	std::copy(rhs._rawdata.begin(),rhs._rawdata.end(),_rawdata.begin());
}
Numor& Numor::operator=(const Numor& rhs)
{
	if (this!=&rhs)
	{
	_number=rhs._number;
	_meta=rhs._meta;
	_rawdata.resize(rhs._rawdata.size());
	std::copy(rhs._rawdata.begin(),rhs._rawdata.end(),_rawdata.begin());
	}
	return *this;
}
int Numor::getNo() const
{
	return _number;
}

bool Numor::validLine(const std::string& str,const char* letter)
{
	return  (!str.compare(0,1,letter) && !str.compare(79,1,letter));
}

void Numor::gotoLine(std::stringstream& buffer, int number,int pos)
{
	int char_number=81*(number-1)+pos;
	buffer.seekg(char_number,std::ios::beg);
}

Numor::~Numor() {
	// TODO Auto-generated destructor stub
}

// Read the file Header, this is all fixed format, not much choice here.
void Numor::readHeader(std::stringstream& buffer)
{
	    // _buffer to contains a full line
		char  line[80];
		// Two temp strings
		std::string s1,s2;
		// Go to Line 2, grab the numor
	    gotoLine(buffer,2,0);
	    buffer >> _number;
	    // Go to Line 6, grab instrument, User, Local contact, date and Time.
	    gotoLine(buffer,6,0);
	    buffer.read(line,80);
	    s1=line;
	    s2=s1.substr(0,4);
	    boost::trim(s2);
	    _meta.add<std::string>("Instrument",s2);
	    s2=s1.substr(4,6);
	    boost::trim(s2);
	    _meta.add<std::string>("User",s2);
	    s2=s1.substr(10,4);
	    boost::trim(s2);
	    _meta.add<std::string>("LocalContact",s2);
	    s2=s1.substr(14,9);
	    _meta.add<std::string>("Date",s2);
	    s2=s1.substr(24,8);
	    _meta.add<std::string>("Time",s2);
	    // Go to Line 10, grab the title and type of scan
		gotoLine(buffer,10,0);
		buffer.read(line,80);
		s1=line;
		s2=s1.substr(0,72);
		boost::trim(s2);
		_meta.add<std::string>("Title",s2);
		s2=s1.substr(72,8);
		boost::trim(s2);
		_meta.add<std::string>("ScanType",s2);
		std::string date, time;

		// Enter a key for the posix time
		date=_meta.getKey<std::string>("Date");
		time=_meta.getKey<std::string>("Time");
		// Now do the job.
		// In mad. the date is written in short UK notation (only 2 digits for the year) DD-MMM-YY
		// Need to convert to ISO format. Assume that no data prior to 2000 needs to be reanalysed.
		std::string full_date=date.substr(0,7)+std::string("20")+date.substr(7,2);
		// Add microsecs to the time to be compliant with boost
		std::string fulltime=time+std::string(".000");
		boost::posix_time::ptime pos_time(boost::gregorian::from_uk_string(full_date),boost::posix_time::duration_from_string(fulltime));
		_meta.add<boost::posix_time::ptime>("ptime",pos_time);
}

const MetaData& Numor::getMetaData() const
{
	return _meta;
}

void Numor::readControlIBlock(std::stringstream& buffer)
{
	// Read the I block, starting at line 12
	gotoLine(buffer,12,0);
	// Read the number of parameters to be read, and number of lines
	int tot, lines;
	buffer >> tot >> lines;
	// Create vector for entries
	std::vector<std::string> Ientries(tot);
	gotoLine(buffer,13,0);
	// Read the metadata keys
	for (int i=0;i<tot;++i)
	{
		buffer >> Ientries[i];
	}
	// Goto the line containing values
	gotoLine(buffer,13+lines,0);
	int value;
	for (int i=0;i<tot;++i)
	{
		buffer >> value;
		// Skip any 0 values in this block
		if (value!=0)
			_meta.add<int>(Ientries[i],value);
	}
	_currentline=13+2*lines;
}

void Numor::readControlFBlock(std::stringstream& buffer)
{
	_currentline++; // Skip the FFFFFFF line.
	gotoLine(buffer,_currentline,0);
	// Total number of entries and number of lines
	int ntot, nlines;
	buffer >> ntot >> nlines;
	// Size of each block
	int sizeblock=16;
	// Number of full lines
	int full_lines=ntot/5;
	// Number of elements in the non full line
	int missing=ntot-5*nlines;
	// Unfortunately this is a fixed format, and there are spaces from some keys
	// need to read.
	char line[80];
	std::string s1,s2;
	std::vector<std::string> keys(ntot);
	int counter=0;

	// read the keys in temp vector
	for (int i=0;i<full_lines;++i)
	{
		gotoLine(buffer,++_currentline,0);
		buffer.read(line,80);
		s1=line;
		for (int j=0;j<5;j++)
		{
			s2=s1.substr(j*sizeblock,sizeblock);
			// Remove all white space from the string (For example H (HMin) and concatenate string
			boost::erase_all(s2," ");
			keys[counter++]=s2;
		}
	} // If non complete line.
	if (missing)
	{
		gotoLine(buffer,++_currentline,0);
		buffer.read(line,80);
		s1=line;
		for (int j=0;j<missing;j++)
		{
			s2=s1.substr(j*sizeblock,sizeblock);
			// Remove all white space
			boost::erase_all(s2," ");
			keys[counter++]=s2;
		}
	}
	double value;
	// Read the values
	gotoLine(buffer,++_currentline,0);
	for (int i=0;i<ntot;++i)
	{
		buffer >> value;
		// Ignore spare member blocks.
		if (keys[i].compare("(spare)"))
			_meta.add<double>(keys[i],value);
	}
	_currentline+=(full_lines+missing);

}

void Numor::readData(std::stringstream& buffer)
{
	_currentline+=3;
	gotoLine(buffer,_currentline,0);
	int npoints;
	buffer >> npoints;
	_rawdata.reserve(npoints);
	gotoLine(buffer,++_currentline,0);
	std::copy(std::istream_iterator<double>(buffer),std::istream_iterator<double>(),std::back_inserter(_rawdata));
}

const std::vector<double>& Numor:: getData() const
{
	return _rawdata;
}

bool Numor::isbpb() const
{
	int kctrl,iflip;
	try
	{
		kctrl=_meta.getKey<int>("kctrl");
		iflip=_meta.getKey<int>("iflip");
		_meta.getKey<int>("nbang");
	}catch(...)
	{
		return 0;
	}
	return  (kctrl==21 && iflip==1);
}

} //end namespace Data

} //end namespace SX
