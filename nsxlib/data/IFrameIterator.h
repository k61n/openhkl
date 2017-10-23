#pragma once

#include "DataTypes.h"
#include "MathematicsTypes.h"

namespace nsx {

class IFrameIterator {
public:
    IFrameIterator(DataSet& data, unsigned int idx): _index(idx), _data(data) {}
    virtual ~IFrameIterator() = default;
    unsigned int index() {return _index;}
    virtual RealMatrix& getFrame() = 0;
    virtual void advance() = 0;

protected:
    unsigned int _index;
    DataSet& _data;
};

} // end namespace nsx
