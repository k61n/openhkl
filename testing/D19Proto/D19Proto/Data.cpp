#include "Data.h"
#include "ILLAsciiDataReader.h"
#include <algorithm>
#include <iostream>

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
    _frames=std::move(mm->getFrame(0));
    _sum[0]=std::accumulate(_frames.begin(),_frames.end(),0,std::plus<int>());
}
void Data::readBlock(int i)
{
    if (!_inmemory)
    {
        _frames=std::move(mm->getFrame(i));
        _sum[i]=std::accumulate(_frames.begin(),_frames.end(),0,std::plus<int>());
    }
    else
    {
        _frames=_data[i];
    }
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
    _inmemory=false;
    _maxCount=0;
}

std::vector<int> Data::getCountsHistogram()
{
    if (!_inmemory)
        readInMemory();
    //
    std::vector<int> histo(_maxCount);

    for (auto i=0;i<_nblocks;++i)
    {
        std::vector<int>& d=_data[i];
        for (auto j=0;j<d.size();++j)
        {
            histo[d[j]]++;
        }
    }
    return histo;
}
