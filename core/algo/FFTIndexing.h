//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/FFTIndexing.h
//! @brief     Declares function findOnSphere
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_ALGO_FFTINDEXING_H
#define OHKL_CORE_ALGO_FFTINDEXING_H

#include "base/geometry/ReciprocalVector.h"

namespace ohkl {

namespace algo {

//! Returns approximately uniformly distributed points on a hemisphere.
std::vector<Eigen::RowVector3d> pointsOnSphere(unsigned int n_vertices);

//! Method to search uniformly on the unit sphere to identify candidate
//! lattice directions. It is crucial to choose reasonable values of
//! n_vertices and nsolutions. Note that the success of FFT indexing is very sensitive to
//! choosing reasonable values of nSubdiva and amax.
std::vector<Eigen::RowVector3d> findOnSphere(
    const std::vector<ReciprocalVector>& qvects, unsigned int n_vertices, unsigned int nsolutions,
    int nSubdiv = 25, double amax = 50.0, double freq_tol = 0.7);

} // namespace algo

} // namespace ohkl

#endif // OHKL_CORE_ALGO_FFTINDEXING_H
