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
    auto it=std::max_element(_currentFrame.begin(),_currentFrame.end());
    _maxCurrentFrame=(*it);
}
void Data::readInMemory()
{
    if (!_inmemory)
    {
        _data.resize(_nblocks);

        int count=0;


#pragma omp parallel for shared(count)
        for (int i=0;i<_nblocks;++i)
        {
            _data[i].reserve(640*256);
            _data[i]=std::move(mm->getFrame(i));
            _sum[i]=std::accumulate(_data[i].begin(),_data[i].end(),0,std::plus<int>());
            auto it=std::max_element(_data[i].begin(),_data[i].end(),std::less<int>());
            if ((*it)>count)
                count=(*it);
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
        readInMemory();
    //
    histo.resize(_maxCount);

    for (auto i=0;i<_nblocks;++i)
    {
        std::vector<int>& d=_data[i];
        for (unsigned int j=0;j<d.size();++j)
        {
            histo[d[j]]++;
        }
    }
}

bool Data::has3DEllipsoid() const
{
    return (_peaks.size()!=0);
}

void Data::clear3DEllipsoids()
{
    _peaks.clear();
}

int Data::dataAt(int x, int y, int z)
{
    if (z<0 || z>=_nblocks || y<0 || y>=256 || x<0 || x>=640)
        return 0;
    return (_data[z])[x*256+y];
}

