#include "EigenToVector.h"

namespace nsx {

std::vector<double> eigenToVector(const Eigen::VectorXd& ev)
{
    const size_t size(ev.size());
    std::vector<double> v(size);
    for (size_t i = 0; i < size; ++i) {
        v[i] = ev(i);
    }
    return v;
}

} // end namespace nsx
