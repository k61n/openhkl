#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>

#include "../data/I16DataReader.h"
#include "../data/TiffDataReader.h"
#include "../instrument/Component.h"
#include "../instrument/ComponentState.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Gonio.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../utils/EigenMatrixParser.h"
#include "../utils/Parser.h"
#include "../utils/Units.h"

namespace SX {
namespace Data {

using SX::Instrument::Diffractometer;
using SX::Instrument::ComponentState;

IDataReader* I16DataReader::create(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer)
{
    return new I16DataReader(filename, diffractometer);
}

I16DataReader::I16DataReader(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer)
: IDataReader(filename,diffractometer)
{
    std::ifstream file;
    std::string dir;
    file.open(filename.c_str(),std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Problem opening file"+filename);
    }
    // Flag for founding metadata
    bool metadata_start=false;
    bool data_start=false;
    int data_count=0;

    while(!file.eof()) {
        std::string line;
        getline(file,line);

        if (line.compare("</MetaDataAtStart>")==0) {
            metadata_start=false;
            getline(file,line);
            getline(file,line);
            getline(file,line);
            data_start=true;
        }
        if (metadata_start)  {
            std::size_t eq=line.find_first_of("=");
            if (eq==std::string::npos) {
                throw std::runtime_error("Could not find symbol = in metadata field");
            }
            std::istringstream is(line.substr(0,eq));
            std::string key;
            is >> key;
            if (key.compare("cmd")==0 || key.compare("date")==0) {
                continue;
            }
            if (key.compare("pilatus100k_path_template")==0) {
                std::string secondary;
                std::istringstream is(line.substr(eq+2,std::string::npos-eq-1));
                is >> secondary;
                dir = boost::filesystem::path(filename).parent_path().string()+"/"+boost::filesystem::path(secondary).parent_path().string();
                continue;
            } else {
                double value;
                std::istringstream is2(line.substr(eq+1,std::string::npos-eq));
                is2 >> value;
                _metadata.add<double>(key,value);
            }
        }
        if (line.compare("<MetaDataAtStart>")==0) {
            metadata_start=true;
        }
        if (data_start) {
            data_count++;
        }
    }
    data_count--;
    _metadata.add<std::string>("Instrument","I16Kappa");
    file.close();
    _tifs.reserve(data_count);
    for (int i=1;i<=data_count;++i) {
        std::ostringstream os;
        os << std::setw(5) << std::setfill('0') << i << ".tif";
        _tifs.push_back(dir+"/"+os.str());
    }
    _nFrames=data_count;

    _states.resize(_nFrames);

    std::vector<double> dval(1);
    dval[0]=_metadata.getKey<double>("delta");
    std::vector<double> sval(3);
    sval[0]=_metadata.getKey<double>("eta");
    sval[1]=_metadata.getKey<double>("chi");
    sval[2]=_metadata.getKey<double>("phi");


    for (unsigned int i=0;i<_nFrames;++i) {
        _states[i].detector = ComponentState(_diffractometer->getDetector().get(), dval);
        _states[i].sample = ComponentState(_diffractometer->getSample().get(), sval);
        //_detectorStates.push_back(_diffractometer->getDetector()->createState(dval));
        //_sampleStates.push_back(_diffractometer->getSample()->createState(sval));
    }

    _metadata.add<int>("Numor",atoi(boost::filesystem::path(filename).stem().string().c_str()));
    _metadata.add<double>("monitor",1.0);
}

void I16DataReader::open()
{
}

void I16DataReader::close()
{
}

Eigen::MatrixXi I16DataReader::getData(size_t frame)
{
    assert(frame<_nFrames);

    TIFF* file=TIFFOpen(_tifs[frame].c_str(),"r");

    Eigen::Matrix<uint32,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> data32(_nRows,_nCols);
    // Read line per line
    for(unsigned short int i=0; i< _nRows; ++i) {
        TIFFReadScanline(file, (char*)&data32(i,0), i);
    }
    TIFFClose(file);
    // Not very nice, but need to copy the 32bits data to int
    return data32.cast<int>();

}

} // end namespace Data
} // end namespace SX
