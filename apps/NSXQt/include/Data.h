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
    int dataAt(int x, int y, int z);
    int _nblocks;
    vint _currentFrame;
    std::vector<vint> _data;
    bool _inmemory;
    int _maxCurrentFrame;
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
