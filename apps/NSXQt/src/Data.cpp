#include "Data.h"
#include "ILLAsciiDataReader.h"
#include <algorithm>
#include <iostream>
#include <QProgressBar>
#include <string>
#include "CylindricalDetector.h"
#include "Units.h"
#include "Gonio.h"
#include <QtDebug>
using namespace SX::Units;

void D19Mapping(double x, double y, double& newx, double& newy)
{
    newx = 640 - x;
    newy = 256 - y;
}

Data::Data():SX::Data::IData(SX::Data::ILLAsciiDataReader::create()),_inmemory(false),_detector(nullptr),_sample(nullptr)
{

}
Data::~Data()
{
    if (_detector)
        delete _detector;
    if (_sample)
        delete _sample;
}

Data::Data(const Data& rhs):SX::Data::IData(SX::Data::ILLAsciiDataReader::create())
{
    _inmemory=rhs._inmemory;
    _detector=rhs._detector;
    _sample=rhs._sample;

}

void Data::fromFile(const std::string& filename)
{
    _mm->open(filename.c_str());
    _nblocks=_mm->nFrames();
    _currentFrame=std::move(_mm->getFrame(0));

    SX::Data::MetaData* meta=_mm->getMetaData();
    std::string instrName = meta->getKey<std::string>("Instrument");

    if (instrName.compare("D19")==0)
    {
        _detector = new SX::Instrument::CylindricalDetector();

        _detector->setDistance(764*SX::Units::mm);
        _detector->setWidthAngle(119.8*deg);
        _detector->setHeight(40.0*cm);
        _detector->setNPixels(640,256);

        _detector->setDataMapping(&D19Mapping);

        _chi=meta->getKey<double>("chi")*deg;
        _phi=meta->getKey<double>("phi")*deg;
        _gamma=meta->getKey<double>("2theta(gamma)")*deg;
        double scanstart=meta->getKey<double>("scanstart")*deg;
        double scanstep=meta->getKey<double>("scanstep")*deg;
        int npdone=meta->getKey<int>("npdone");

        _omegas.resize(npdone);
        int i=0;
        std::generate(_omegas.begin(),_omegas.end(),[&i,scanstart,scanstep](){return scanstart+(i++)*scanstep;});

        _wavelength = meta->getKey<double>("wavelength");

        // Attach a gonio to the detector
        std::shared_ptr<SX::Instrument::Gonio> g(new SX::Instrument::Gonio("gamma-arm"));
        g->addRotation("gamma",Eigen::Vector3d(0,0,1),SX::Instrument::RotAxis::CW);
        g->addTranslation("y-offset",Eigen::Vector3d(0,1,0));
        _detector->setGonio(g);

        //Sample gonio
        _sample= new SX::Instrument::Sample();
        std::shared_ptr<SX::Instrument::Gonio> bl(new SX::Instrument::Gonio("Busing-Levy"));
        bl->addRotation("omega",Vector3d(0,0,1),SX::Instrument::RotAxis::CW);
        bl->addRotation("chi",Vector3d(0,1,0),SX::Instrument::RotAxis::CCW);
        bl->addRotation("phi",Vector3d(0,0,1),SX::Instrument::RotAxis::CW);
        bl->addTranslation("x-sample",Vector3d(1,0,0));
        bl->addTranslation("y-sample",Vector3d(0,1,0));
        bl->addTranslation("z-sample",Vector3d(0,0,1));
        _sample->setGonio(bl);
    }

}

void Data::readBlock(int i)
{
    if (!_inmemory)
    {
        _currentFrame=std::move(_mm->getFrame(i));
    }
    else
    {
        _currentFrame=_data[i];
    }
    auto it=std::max_element(_currentFrame.begin(),_currentFrame.end());
    _maxCurrentFrame=(*it);
}
void Data::readInMemory()
{

    if (!_inmemory)
    {
        _data.resize(_nblocks);
        _inmemory=true;
#pragma omp parallel for
        for (int i=0;i<_nblocks;++i)
        {
            _data[i].reserve(_detector->getNCols()*_detector->getNRows());
            _data[i]=std::move(_mm->getFrame(i));
        }
    }
}
// Release the Data from memory
void Data::releaseMemory()
{
    if (!_inmemory)
        return;
    for (auto i=0;i<_nblocks;++i)
    {
        _data[i].clear();
    }
    _data.clear();
    _inmemory=false;
}

int Data::dataAt(int x, int y, int z)
{
    if (z<0 || z>=_nblocks || y<0 || y>=_detector->getNRows() || x<0 || x>=_detector->getNCols())
        return 0;
    return (_data[z])[x*_detector->getNRows()+y];
}

