#ifndef CORE_AUTO_INDEXING_FFTINDEXING_H
#define CORE_AUTO_INDEXING_FFTINDEXING_H

#include <functional>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "GeometryTypes.h"
#include "Units.h"

namespace nsx {

//! \class FFTIndexing
//! \brief Class used for Fast-Fourier transform autoindexing.
class FFTIndexing {
public:
    //! Type to store vector together with quality.
    using tVector = std::pair<Eigen::RowVector3d, double>;
    //! Constructor. Note that the success of FFT indexing is very sensitive to
    //! choosing reasonable values of nSubdiva and amax.
    FFTIndexing(int nSubdiv = 25, double amax = 50.0);
    //! Method to search uniformly on the unit sphere to identify candidate
    //! lattice directions. It is crucial to choose reasonable values of
    //! n_vertices and nsolutions.
    std::vector<tVector> findOnSphere(
        const std::vector<ReciprocalVector>& qvects, unsigned int n_vertices,
        unsigned int nsolutions) const;
    virtual ~FFTIndexing() = default;

private:
    int _nSubdiv;
    double _amax;
};

} // end namespace nsx

#endif // CORE_AUTO_INDEXING_FFTINDEXING_H
