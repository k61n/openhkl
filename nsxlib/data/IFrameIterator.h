// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_IFRAMEITERATOR_H_
#define NSXTOOL_IFRAMEITERATOR_H_

#include <functional>
#include "../utils/Types.h"



namespace SX {
namespace Data {

class IData;

class IFrameIterator {
public:
    IFrameIterator(IData& data, unsigned int idx): _index(idx), _data(data) {}
    virtual ~IFrameIterator() = default;
    unsigned int index() {return _index;}
    virtual SX::Types::RealMatrix& getFrame() = 0;
    virtual void advance() = 0;

protected:
    unsigned int _index;
    IData& _data;
};

} // namespace Data
} // namespace SX

#endif // NSXTOOL_IFRAME_ITERATOR_H_
