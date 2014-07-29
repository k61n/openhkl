#ifndef DATA_H
#define DATA_H
#include <vector>
#include <string>
#include "IDataReader.h"
#include <memory>
#include <map>
#include <Ellipsoid.h>
#include <QVector>
#include "Peak3D.h"

typedef std::vector<int> vint;


class QProgressBar;

struct Data: public SX::Geometry::IData
{
    Data();
    ~Data();
    void fromFile(const std::string& filename);
    void readBlock(int i);
    void readInMemory();
    // Release the data from memory
    void releaseMemory();
    // Get the histogram of intensity distribution
    void getCountsHistogram(std::vector<int>&);
    // True if 3D peaks are present
    bool has3DEllipsoid() const;
    // clear
    void clear3DEllipsoids();
    int getBackgroundLevel();
    int dataAt(int x, int y, int z);
    int _nblocks;
    std::unique_ptr<SX::Data::IDataReader> mm;
    vint _currentFrame;
    std::vector<vint> _data;
    std::vector<int> _sum;
    bool _inmemory;
    int _maxCount;
    int _maxCurrentFrame;
    std::map<int,SX::Geometry::Ellipsoid<double,3>,std::less<int>, Eigen::aligned_allocator<std::pair<const int, Eigen::Matrix4d> >> _peaks;
    typedef std::map<int,SX::Geometry::Peak3D> maprealPeaks;
    maprealPeaks _rpeaks;
};

#endif // DATA_H
