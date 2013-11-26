#include "ILLAsciiMetaReader.h"
#include "MetaData.h"
#include <stdexcept>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <string>

namespace SX
{

namespace Data
{

int ILLAsciiMetaReader::_currentline=0;

MetaData* ILLAsciiMetaReader::read(const char* buf,std::size_t& size)
{

	 std::string s(buf);
	 std::size_t end_metadata=s.find("SSSSSSSS");

	 if (end_metadata==std::string::npos)
	    throw std::runtime_error("Could not find end of metadata block");

	 // Number of characters up to the "SSSS...." line
	 size=end_metadata;

	_currentline=0;
	MetaData* meta=new MetaData;
	std::stringstream buffer;
	buffer << s.substr(0,end_metadata);
	// Try to read the header file
	try
	{
		readHeader(buffer,meta);
	}catch(...)
	{
		throw std::runtime_error("ILLAsciiMetaReader: Fail to read Header in stream");
	}

	// Read the block containing control parameters (integer)
	try
	{
		readControlIBlock(buffer,meta);
	}catch(...)
	{
		throw std::runtime_error("ILLAsciiMetaEeader: Fail to read IBlock in stream");
	}

	// Read the block containing float metadata
	try
	{
		readControlFBlock(buffer,meta);
	}catch(...)
	{
		throw std::runtime_error("ILLAsciiMetaReader: Fail to read FBlock in stream");
	}
	return meta;
}


void ILLAsciiMetaReader::gotoLine(std::stringstream& buffer, int number,int pos)
{
	int char_number=81*(number-1)+pos;
	buffer.seekg(char_number,std::ios::beg);
}


// Read the file Header, this is all fixed format, not much choice here.
void ILLAsciiMetaReader::readHeader(std::stringstream& buffer, MetaData* meta)
{
	    // _buffer to contains a full line
		char  line[80];
		// Two temp strings
		std::string s1,s2;
		// Go to Line 2, grab the numor
	    gotoLine(buffer,2,0);
	    int _number;
	    buffer >> _number;
	    //
	    meta->add<int>("Numor",_number);
	    // Go to Line 6, grab instrument, User, Local contact, date and Time.
	    gotoLine(buffer,6,0);
	    buffer.read(line,80);
	    s1=line;
	    s2=s1.substr(0,4);
	    boost::trim(s2);
	    meta->add<std::string>("Instrument",s2);
	    s2=s1.substr(4,6);
	    boost::trim(s2);
	    meta->add<std::string>("User",s2);
	    s2=s1.substr(10,4);
	    boost::trim(s2);
	    meta->add<std::string>("LocalContact",s2);
	    s2=s1.substr(14,9);
	    meta->add<std::string>("Date",s2);
	    s2=s1.substr(24,8);
	    meta->add<std::string>("Time",s2);
	    // Go to Line 10, grab the title and type of scan
		gotoLine(buffer,10,0);
		buffer.read(line,80);
		s1=line;
		s2=s1.substr(0,72);
		boost::trim(s2);
		meta->add<std::string>("Title",s2);
		s2=s1.substr(72,8);
		boost::trim(s2);
		meta->add<std::string>("ScanType",s2);
		std::string date, time;

		// Enter a key for the posix time
		date=meta->getKey<std::string>("Date");
		time=meta->getKey<std::string>("Time");
		// Now do the job.
		// In mad. the date is written in short UK notation (only 2 digits for the year) DD-MMM-YY
		// Need to convert to ISO format. Assume that no data prior to 2000 needs to be reanalysed.
		std::string full_date=date.substr(0,7)+std::string("20")+date.substr(7,2);
		// Add microsecs to the time to be compliant with boost
		std::string fulltime=time+std::string(".000");
		boost::posix_time::ptime pos_time(boost::gregorian::from_uk_string(full_date),boost::posix_time::duration_from_string(fulltime));
		meta->add<boost::posix_time::ptime>("ptime",pos_time);
}

void ILLAsciiMetaReader::readControlIBlock(std::stringstream& buffer,MetaData* meta)
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
			meta->add<int>(Ientries[i],value);
	}
	_currentline=13+2*lines;
}

void ILLAsciiMetaReader::readControlFBlock(std::stringstream& buffer,MetaData* meta)
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
			meta->add<double>(keys[i],value);
	}
	_currentline+=(full_lines+missing);

}

} // end namespace Data

} // end namespace SX
