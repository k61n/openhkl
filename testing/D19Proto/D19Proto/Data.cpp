#include "Data.h"
#include "ILLAsciiDataReader.h"
#include <algorithm>
#include <iostream>
#include <QProgressBar>



Data::Data():mm(SX::Data::ILLAsciiDataReader::create()),_inmemory(false),_maxCount(0)
{

}
Data::~Data()
{
}

void Data::fromFile(const std::string& filename)
{
    mm->open(filename.c_str());
    _nblocks=mm->nFrames();
    _sum.resize(_nblocks);
    _currentFrame=std::move(mm->getFrame(0));
    _sum[0]=std::accumulate(_currentFrame.begin(),_currentFrame.end(),0,std::plus<int>());
}
void Data::readBlock(int i)
{
    if (!_inmemory)
    {
        _currentFrame=std::move(mm->getFrame(i));
        _sum[i]=std::accumulate(_currentFrame.begin(),_currentFrame.end(),0,std::plus<int>());
    }
    else
    {
        _currentFrame=_data[i];
    }
}
void Data::readInMemory(QProgressBar* bar)
{
    if (!_inmemory)
    {
        _data.resize(_nblocks);

        int count=0;

int counter=0;


#pragma omp parallel for shared(count)
        for (int i=0;i<_nblocks;++i)
        {
            _data[i].reserve(640*256);
            _data[i]=std::move(mm->getFrame(i));
            _sum[i]=std::accumulate(_data[i].begin(),_data[i].end(),0,std::plus<int>());
            auto it=std::max_element(_data[i].begin(),_data[i].end(),std::less<int>());
            if ((*it)>count)
                count=(*it);
            if (bar)
            {
#pragma omp critical
                {
                int progress=static_cast<int>(100.0*counter++/_nblocks);
                //bar->setFormat("Reading Numor: "+QString::number(progress)+"%");
                //bar->setValue(progress);
                }
            }
        }
        if (bar)
        {
            bar->setFormat("");
            bar->setValue(0);
            bar->setEnabled(false);
        }
        _maxCount=count;
        _inmemory=true;
    }
}
// Release the Data from memory
void Data::releaseMemory()
{
    if (!_inmemory)
        return;
    for (auto i=0;i<_nblocks;++i)
    {
        _data[i].empty();
    }
    _data.empty();
    _sum.empty();
    _inmemory=false;
    _maxCount=0;
}

void Data::getCountsHistogram(std::vector<int>& histo)
{
    if (!_inmemory)
        readInMemory(nullptr);
    //
    histo.resize(_maxCount);

    for (auto i=0;i<_nblocks;++i)
    {
        std::vector<int>& d=_data[i];
        for (auto j=0;j<d.size();++j)
        {
            histo[d[j]]++;
        }
    }
}
