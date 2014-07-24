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


    int _nblocks;
    std::unique_ptr<SX::Data::IDataReader> mm;
    vint _frames;
    std::vector<vint> _data;
    bool _inmemory;

};

#endif // DATA_H
