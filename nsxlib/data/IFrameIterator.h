// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_IFRAMEITERATOR_H_
#define NSXTOOL_IFRAMEITERATOR_H_

#include <functional>
#include "../utils/Types.h"



namespace nsx {
namespace Data {

class DataSet;

class IFrameIterator {
public:
    IFrameIterator(DataSet& data, unsigned int idx): _index(idx), _data(data) {}
    virtual ~IFrameIterator() = default;
    unsigned int index() {return _index;}
    virtual nsx::Types::RealMatrix& getFrame() = 0;
    virtual void advance() = 0;

protected:
    unsigned int _index;
    DataSet& _data;
};

} // namespace Data
} // namespace nsx

#endif // NSXTOOL_IFRAME_ITERATOR_H_
