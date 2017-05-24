// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXLIB_IFRAMEITERATOR_H
#define NSXLIB_IFRAMEITERATOR_H

#include "../data/DataTypes.h"
#include "../mathematics/MathematicsTypes.h"

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

#endif // NSXLIB_IFRAMEITERATOR_H
