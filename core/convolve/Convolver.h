//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#ifndef NSX_CORE_CONVOLVE_CONVOLVER_H
#define NSX_CORE_CONVOLVE_CONVOLVER_H

#include <Eigen/Dense>
#include <map>

namespace nsx {

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

//! Pure virtual base class of all convolvers (= image filters).

class Convolver {
 public:
    Convolver();

    Convolver(const std::map<std::string, double>& parameters);

    Convolver(const Convolver& other) = default;

    Convolver& operator=(const Convolver& other) = default;

    virtual ~Convolver() = 0;

    virtual Convolver* clone() const = 0;

    //! Non-const getter for kernel parameter
    std::map<std::string, double>& parameters();

    //! Const getter for kernel parameter
    const std::map<std::string, double>& parameters() const;
    //! Set convolver parameters
    void setParameters(const std::map<std::string, double>& parameters);
    //! Set the type of the convolver (informative name)
    void setType(std::string type) { _convolver_type = type; };
    //! get the name of the convolver
    std::string type() const { return _convolver_type; };
    //! Convolve an image
    virtual RealMatrix convolve(const RealMatrix& image) = 0;

    virtual std::pair<size_t, size_t> kernelSize() const = 0;

 protected:
    std::map<std::string, double> _parameters;
    std::string _convolver_type = "";
};

} // namespace nsx

#endif // NSX_CORE_CONVOLVE_CONVOLVER_H
