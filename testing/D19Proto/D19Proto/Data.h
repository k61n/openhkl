#ifndef DATA_H
#define DATA_H
#include <vector>
#include <string>
#include "IDataReader.h"
#include <memory>
#include <map>
#include <Ellipsoid.h>

typedef std::vector<int> vint;


class QProgressBar;

struct Data
{
    Data();
    ~Data();
    void fromFile(const std::string& filename);
    void readBlock(int i);
    void readInMemory(QProgressBar*);
    // Release the data from memory
    void releaseMemory();
    // Get the histogram of intensity distribution
    void getCountsHistogram(std::vector<int>&);

    int _nblocks;
    std::unique_ptr<SX::Data::IDataReader> mm;
    vint _currentFrame;
    std::vector<vint> _data;
    std::vector<int> _sum;
    int _maxCount;
    bool _inmemory;
    std::map<int,SX::Geometry::Ellipsoid<double,3>,std::less<int>, Eigen::aligned_allocator<std::pair<const int, Eigen::Matrix4d> >> _peaks;

};

#endif // DATA_H
