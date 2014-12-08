#include <cmath>
#include <cstring>
#include <stdexcept>

#include "EigenMatrixParser.h"
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/file_mapping.hpp>


#include "ILLAsciiData.h"
#include "Component.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Gonio.h"
#include "Parser.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace SX
{

namespace Data
{

using namespace SX::Utils;
using namespace SX::Units;

// 81 characters per line, at least 100 lines of header
std::size_t ILLAsciiData::BlockSize=100*81;

std::map<std::size_t,std::string> ILLAsciiData::MADAngles = {{1,"2theta(gamma)"},{2,"omega"},{3,"chi"},{4,"phi"},{5,"nu"}};

IData* ILLAsciiData::create(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer)
{
	return new ILLAsciiData(filename,diffractometer);
}

ILLAsciiData::ILLAsciiData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer)
: IData(filename,diffractometer)
{
	try
	{
		boost::interprocess::file_mapping filemap(_filename.c_str(), boost::interprocess::read_only);
		_map=boost::interprocess::mapped_region(filemap,boost::interprocess::read_only);
	}
	catch(...)
	{
		throw;
	}

	// Beginning of the blockILLAsciiDataReader
	_mapAddress=reinterpret_cast<char*>(_map.get_address());

	char* buffer=new char[BlockSize];
	strncpy(buffer, _mapAddress, BlockSize);
	readMetaData(buffer);
	delete [] buffer;
	// Extract some variables from the metadata
	_nFrames=_metadata->getKey<int>("npdone");
	_dataPoints=_metadata->getKey<int>("nbdata");
	_nAngles=_metadata->getKey<int>("nbang");
	// Skip 8 or 9 lines to the beginning of data blocks
	_skipChar=81*(8+(_nAngles<=2 ? 0 : 1));
	// ILL Ascii file for 2D detector store 10 values per line.
	_dataLength=static_cast<int>(std::ceil(_dataPoints/10.0))*81;

	// Get the value of the monitor for the first frame
	std::vector<int> vi;
	std::size_t skip3Lines=3*81;
	readIntsFromChar(_mapAddress+_headerSize+skip3Lines,_mapAddress+_headerSize+skip3Lines+16,vi);
	if ((int)(vi.size() != 2) || (vi[0] != (int)(_nAngles+3)))
		throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock 1.");


	std::size_t fromStoFData=skip3Lines + (vi[1]+1)*81;

	const char* beginValues=_mapAddress+_headerSize+fromStoFData;
	std::vector<double> vd;
	std::size_t FData=81*static_cast<int>(std::ceil(vi[0]/6.0));
	readDoublesFromChar(beginValues,beginValues+FData,vd);

	if (vd.size() != (_nAngles+3))
		throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock 2.");

	_metadata->add<double>("monitor",vd[1]);

	std::vector<std::string> scans(_nAngles);

	for (std::size_t i=0;i<_nAngles;++i)
	{
		std::string idesc = std::string("icdesc") + std::to_string(i+1);
		int idx = _metadata->getKey<int>(idesc);
		scans[i] = MADAngles[idx];
	}

	std::vector<double*> varAngles(_nAngles);

	std::vector<std::string> dAxisNames(_diffractometer->getDetector()->getGonio()->getPhysicalAxesNames());
	std::vector<double> dval(dAxisNames.size());
	int comp=0;
	for (const auto& n : dAxisNames)
	{
		auto it=std::find(scans.begin(),scans.end(),n);
		if (it == scans.end())
			dval[comp]=_metadata->getKey<double>(n)*deg;
		else
			varAngles[std::distance(scans.begin(),it)] = &dval[comp];
		comp++;
	}

	std::vector<std::string> sAxisNames(_diffractometer->getSample()->getGonio()->getPhysicalAxesNames());
	std::vector<double> sval(sAxisNames.size());
	comp=0;
	for (const auto& n : sAxisNames)
	{
		auto it=std::find(scans.begin(),scans.end(),n);
		if (it == scans.end())
		{
			sval[comp]=_metadata->getKey<double>(n)*deg;
		}
		else
			varAngles[std::distance(scans.begin(),it)] = &sval[comp];
		comp++;
	}

	_detectorStates.reserve(_nFrames);
	_sampleStates.reserve(_nFrames);

	const char* start = _mapAddress+_headerSize;

	std::size_t frameB=0,frameE=_nFrames-1;

	std::vector<double> bb,be;

	beginValues = start  + frameB*(_dataLength+_skipChar) + fromStoFData;
	readDoublesFromChar(beginValues,beginValues+FData,bb);

	if (bb.size() != (_nAngles+3))
		throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock 2.");


	beginValues = start  + frameE*(_dataLength+_skipChar) + fromStoFData;
		readDoublesFromChar(beginValues,beginValues+FData,be);

		if (be.size() != (_nAngles+3))
			throw std::runtime_error("Problem parsing numor: mismatch between number of angles in header and datablock 2.");


	for (std::size_t f=0;f<_nFrames;++f)
	{
		for (std::size_t i=0;i<_nAngles;++i)
			*(varAngles[i]) = (bb[3+i]+(be[3+i]-bb[3+i])*static_cast<double>(f)/static_cast<double>(_nFrames-1))*deg/1000.0;

		_detectorStates.push_back(_diffractometer->getDetector()->createState(dval));
		_sampleStates.push_back(_diffractometer->getSample()->createState(sval));
	}
	_fileSize=_map.get_size();

	close();
}

ILLAsciiData::~ILLAsciiData() {
}

void ILLAsciiData::open()
{
	if (_isOpened)
		return;
	try
	{
		boost::interprocess::file_mapping filemap(_filename.c_str(), boost::interprocess::read_only);
		boost::interprocess::mapped_region reg(filemap,boost::interprocess::read_only);
		_map.swap(reg);
		_mapAddress=reinterpret_cast<char*>(_map.get_address());
	}
	catch(...)
	{
		throw;
	}
	_isOpened=true;
}

void ILLAsciiData::close()
{
	if (!_isOpened)
		return;
	_map=boost::move(boost::interprocess::mapped_region());
	_isOpened=false;
}

Eigen::MatrixXi ILLAsciiData::getFrame(std::size_t idx)
{

	if (_inMemory)
		return _data[idx];
	else
		return readFrame(idx);
}

Eigen::MatrixXi ILLAsciiData::readFrame(std::size_t idx)
{
	assert(idx<_nFrames);

	// Determine the beginning of the data block
	std::size_t begin=_headerSize+(idx+1)*_skipChar+idx*_dataLength;

	// Create vector and try to reserve a memory block
	Eigen::MatrixXi v;
	v.resize(_nrows,_ncols);

	EigenMatrixParser<const char*,TopRightColMajorMapper> parser;
	qi::phrase_parse(_mapAddress+begin,_mapAddress+begin+_dataLength,parser,qi::blank, v);

	return v;
}

void ILLAsciiData::readInMemory()
{

	if (_inMemory)
        return;

	if (!_isOpened)
		open();

	_data.resize(_nFrames);

	#pragma omp parallel for
	for (std::size_t i=0;i<_nFrames;++i)
		_data[i]=readFrame(i);

	_inMemory=true;

	return;
}

void ILLAsciiData::goToLine(std::stringstream& buffer, int number,int pos)
{
	int char_number=81*(number-1)+pos;
	buffer.seekg(char_number,std::ios::beg);
}

void ILLAsciiData::readControlIBlock(std::stringstream& buffer)
{
	// Read the I block, starting at line 12
	goToLine(buffer,12,0);

	// Read the number of parameters to be read, and number of lines
	int tot, lines;
	buffer >> tot >> lines;

	// Create vector for entries
	std::vector<std::string> Ientries(tot);
	goToLine(buffer,13,0);

	// Read the metadata keys
	for (int i=0;i<tot;++i)
		buffer >> Ientries[i];

	// Goto the line containing values
	goToLine(buffer,13+lines,0);
	int value;
	for (int i=0;i<tot;++i)
	{
		buffer >> value;
		// Skip any 0 values in this block
		if (value!=0)
			_metadata->add<int>(Ientries[i],value);
	}
	_currentLine=13+2*lines;
}

void ILLAsciiData::readControlFBlock(std::stringstream& buffer)
{
	_currentLine++; // Skip the FFFFFFF line.
	goToLine(buffer,_currentLine,0);
	// Total number of entries and number of lines
	int nTot, nLines;
	buffer >> nTot >> nLines;
	// Size of each block
	int sizeBlock=16;
	// Number of full lines
	int fullLines=nTot/5;
	// Number of elements in the non full line
	int missing=nTot-5*nLines;
	// Unfortunately this is a fixed format, and there are spaces from some keys
	// need to read.
	char line[80];
	std::string s1,s2;
	std::vector<std::string> keys(nTot);
	int counter=0;

	// read the keys in temp vector
	for (int i=0;i<fullLines;++i)
	{
		goToLine(buffer,++_currentLine,0);
		buffer.read(line,80);
		s1=line;
		for (int j=0;j<5;j++)
		{
			s2=s1.substr(j*sizeBlock,sizeBlock);
			// Remove all white space from the string (For example H (HMin) and concatenate string
			boost::erase_all(s2," ");
			keys[counter++]=s2;
		}
	}

	// If non complete line.
	if (missing)
	{
		goToLine(buffer,++_currentLine,0);
		buffer.read(line,80);
		s1=line;
		for (int j=0;j<missing;j++)
		{
			s2=s1.substr(j*sizeBlock,sizeBlock);
			// Remove all white space
			boost::erase_all(s2," ");
			keys[counter++]=s2;
		}
	}

	double value;
	// Read the values
	goToLine(buffer,++_currentLine,0);
	for (int i=0;i<nTot;++i)
	{
		buffer >> value;
		// Ignore spare member blocks.
		if (keys[i].compare("(spare)"))
			_metadata->add<double>(keys[i],value);
	}
	_currentLine+=(fullLines+missing);

    _diffractometer->getSource()->setWavelength(_metadata->getKey<double>("wavelength"));

}

void ILLAsciiData::readHeader(std::stringstream& buffer)
{
	// _buffer to contains a full line
	char  line[80];
	// Two temp strings
	std::string s1,s2;
	// Go to Line 2, grab the numor
	goToLine(buffer,2,0);
	int number;
	buffer >> number;

	_metadata->add<int>("Numor",number);

	// Go to Line 6, grab instrument, User, Local contact, date and Time.
	goToLine(buffer,6,0);
	buffer.read(line,80);
	s1=line;
	s2=s1.substr(0,4);
	boost::trim(s2);
	_metadata->add<std::string>("Instrument",s2);
	s2=s1.substr(4,6);
	boost::trim(s2);
	_metadata->add<std::string>("User",s2);
	s2=s1.substr(10,4);
	boost::trim(s2);
	_metadata->add<std::string>("LocalContact",s2);
	s2=s1.substr(14,9);
	_metadata->add<std::string>("Date",s2);
	s2=s1.substr(24,8);
	_metadata->add<std::string>("Time",s2);
	// Go to Line 10, grab the title and type of scan
	goToLine(buffer,10,0);
	buffer.read(line,80);
	s1=line;
	s2=s1.substr(0,72);
	boost::trim(s2);
	_metadata->add<std::string>("Title",s2);
	s2=s1.substr(72,8);
	boost::trim(s2);
	_metadata->add<std::string>("ScanType",s2);
	std::string date, time;

	// Enter a key for the posix time
	date=_metadata->getKey<std::string>("Date");
	time=_metadata->getKey<std::string>("Time");
	// Now do the job.
	// In mad. the date is written in short UK notation (only 2 digits for the year) DD-MMM-YY
	// Need to convert to ISO format. Assume that no data prior to 2000 needs to be reanalysed.
	std::string full_date=date.substr(0,7)+std::string("20")+date.substr(7,2);
	// Add microsecs to the time to be compliant with boost
	std::string fulltime=time+std::string(".000");
	boost::posix_time::ptime pos_time(boost::gregorian::from_uk_string(full_date),boost::posix_time::duration_from_string(fulltime));
	_metadata->add<boost::posix_time::ptime>("ptime",pos_time);

}

void ILLAsciiData::readMetaData(const char* buf)
{

	std::string s(buf);
	std::size_t endMetadata=s.find("SSSSSSSS");

	if (endMetadata==std::string::npos)
	throw std::runtime_error("Could not find end of metadata block");

	// Number of characters up to the "SSSS...." line
	_headerSize = endMetadata;

	_currentLine=0;

	std::stringstream buffer;
	buffer << s.substr(0,endMetadata);
	// Try to read the header file
	try
	{
		readHeader(buffer);
	}catch(...)
	{
		throw std::runtime_error("ILLAsciiData: Fail to read Header in stream");
	}

	// Read the block containing control parameters (integer)
	try
	{
		readControlIBlock(buffer);
	}catch(...)
	{
		throw std::runtime_error("ILLAsciiMetaReader: Fail to read IBlock in stream");
	}

	// Read the block containing float metadata
	try
	{
		readControlFBlock(buffer);
	}catch(...)
	{
		throw std::runtime_error("ILLAsciiMetaReader: Fail to read FBlock in stream");
	}
}

} // end namespace Data

} // end namespace SX
