#ifndef DATA_H
#define DATA_H
#include <vector>
#include <string>
#include "IDataReader.h"
#include <memory>
typedef std::vector<int> vint;

struct Data
{
    Data();
    ~Data();
    void fromFile(const std::string& filename);
    void readBlock(int i);
    void readInMemory();
    // Release the data from memory
    void releaseMemory();
    // Get the histogram of intensity distribution
    std::vector<int> getCountsHistogram();

    int _nblocks;
    std::unique_ptr<SX::Data::IDataReader> mm;
    vint _frames;
    std::vector<vint> _data;
    std::vector<int> _sum;
    int _maxCount;
    bool _inmemory;

};

#endif // DATA_H
