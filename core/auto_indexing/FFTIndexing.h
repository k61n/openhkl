//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/auto_indexing/FFTIndexing.h
//! @brief     Defines class FFTIndexing
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_AUTO_INDEXING_FFTINDEXING_H
#define CORE_AUTO_INDEXING_FFTINDEXING_H

#include "core/geometry/ReciprocalVector.h"

namespace nsx {

//! Class used for Fast-Fourier transform autoindexing.
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

} // namespace nsx

#endif // CORE_AUTO_INDEXING_FFTINDEXING_H
