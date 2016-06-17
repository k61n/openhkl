// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_IFRAMEITERATOR_H_
#define NSXTOOL_IFRAMEITERATOR_H_

#include <functional>
#include "Types.h"



namespace SX {
namespace Data {

class IData;

class IFrameIterator {
public:
    IFrameIterator(IData* data, int idx) {};
    virtual ~IFrameIterator() {};

    virtual SX::Types::RealMatrix& getFrame() = 0;
    virtual void advance() = 0;
    virtual int index() = 0;
};

} // namespace Data
} // namespace SX

#endif // NSXTOOL_IFRAME_ITERATOR_H_
