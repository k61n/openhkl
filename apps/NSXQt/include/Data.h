#ifndef DATA_H
#define DATA_H
#include <vector>
#include <string>
#include "IData.h"
#include <memory>
#include <map>
#include <Ellipsoid.h>
#include <QVector>
#include "Peak3D.h"
#include "Sample.h"


typedef std::vector<int> vint;

namespace SX
{
namespace Instrument
{
class Detector;
}
}


class QProgressBar;

struct Data: public SX::Data::IData
{
    Data();
    ~Data();
    Data(const Data& rhs);
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
    vint _currentFrame;
    std::vector<vint> _data;
    std::vector<int> _sum;
    bool _inmemory;
    int _maxCount;
    int _maxCurrentFrame;
    std::map<int,SX::Geometry::Ellipsoid<double,3>,std::less<int>, Eigen::aligned_allocator<std::pair<const int, Eigen::Matrix4d> >> _peaks;
    typedef std::map<int,SX::Crystal::Peak3D> maprealPeaks;
    maprealPeaks _rpeaks;
    SX::Instrument::Detector* _detector;
    SX::Instrument::Sample* _sample;
    double _wavelength;
    double _gamma;
    double _chi;
    double _phi;
    std::vector<double> _omegas;

};

#endif // DATA_H
