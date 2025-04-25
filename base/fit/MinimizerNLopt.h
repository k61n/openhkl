//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/fit/MinimizerNLopt.h
//! @brief     Defines class MinimizerNLopt
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_FIT_MINIMIZERNLOPT_H
#define OHKL_BASE_FIT_MINIMIZERNLOPT_H

#include <nlopt.hpp>

namespace ohkl {

struct NLoptFitData {
    NLoptFitData(unsigned int size);

    std::size_t npoints;
    std::vector<double> x_data;
    std::vector<double> y_data;
};

//! Wraps the NLopt non-linear least squares minimization routines.
class MinimizerNLopt {
 public:
    explicit MinimizerNLopt(unsigned int nparams, unsigned int ndatapoints);

    void reset();
    void setObjectiveFunction(nlopt::vfunc func);
    void setFTol(double ftol);
    NLoptFitData* data() { return &_data; };
    bool minimize(std::vector<double>& parameters);

 private:
    unsigned int _nparams;
    nlopt::algorithm _algo;
    nlopt::opt _optimizer;
    double _ftol;
    int _max_iter;
    NLoptFitData _data;
};

} // namespace ohkl

#endif // OHKL_BASE_FIT_MINIMIZERNLOPT_H
