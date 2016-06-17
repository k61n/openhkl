// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_THREADEDFRAMEITERATOR_H_
#define NSXTOOL_THREADEDFRAMEITERATOR_H_

#include "IData.h"
#include "IFrameIterator.h"

namespace SX {

namespace Data {
/*
class ThreadedFrameIterator {
public:
    ThreadedFrameIterator(IData* parent, int idx=0, std::launch policy=std::launch::async);
    ThreadedFrameIterator(const ThreadedFrameIterator& other);

    ThreadedFrameIterator& operator++();

    bool operator!=(const ThreadedFrameIterator& other) const;
    bool operator==(const ThreadedFrameIterator& other) const;

    Eigen::MatrixXi& operator*();
    Eigen::MatrixXi* operator->();

private:
    std::shared_future<Eigen::MatrixXi> getFrame(int idx);

    int _currentFrame;
    IData* _parent;
    Eigen::MatrixXi _currentData;
    std::shared_future<Eigen::MatrixXi> _nextData;
    std::launch _launchPolicy;
};
*/
} // Data

} // SX

#endif // NSXTOOL_THREADEDFRAMEITERATOR_H_
