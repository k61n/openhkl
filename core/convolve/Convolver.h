//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/convolve/Convolver.h
//! @brief     Defines class Convolver
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_SEARCH_PEAKS_CONVOLVER_H
#define CORE_SEARCH_PEAKS_CONVOLVER_H

#include <map>
#include <Eigen/Dense>

namespace nsx {

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

class Convolver {

public:
    Convolver();

    Convolver(const std::map<std::string, double>& parameters);

    Convolver(const Convolver& other) = default;

    Convolver& operator=(const Convolver& other) = default;

    virtual ~Convolver() = 0;

    virtual Convolver* clone() const = 0;

    // Non-const getter for kernel parameter
    std::map<std::string, double>& parameters();

    // Const getter for kernel parameter
    const std::map<std::string, double>& parameters() const;

    void setParameters(const std::map<std::string, double>& parameters);

    //! Convolve an image
    virtual RealMatrix convolve(const RealMatrix& image) = 0;

    virtual std::pair<size_t, size_t> kernelSize() const = 0;

protected:
    std::map<std::string, double> _parameters;
};

} // namespace nsx

#endif // CORE_SEARCH_PEAKS_CONVOLVER_H
