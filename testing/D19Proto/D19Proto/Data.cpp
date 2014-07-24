#include "Data.h"
#include "ILLAsciiDataReader.h"

Data::Data():mm(SX::Data::ILLAsciiDataReader::create()),_inmemory(false)
{

}
Data::~Data()
{
}

void Data::fromFile(const std::string& filename)
{
    mm->open(filename.c_str());
    _nblocks=mm->nFrames();
    _frames=std::move(mm->getFrame(0));

}
void Data::readBlock(int i)
{
    if (!_inmemory)
    {
        _frames=std::move(mm->getFrame(i));
    }
    else
        _frames=_data[i];
}
void Data::readInMemory()
{
    if (!_inmemory)
    {
        _data.resize(_nblocks);
        #pragma omp parallel for
        for (int i=0;i<_nblocks;++i)
        {
            _data[i].reserve(640*256);
            _data[i]=std::move(mm->getFrame(i));
        }
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
}
