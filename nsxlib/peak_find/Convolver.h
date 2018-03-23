/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <complex>
#include <ostream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include <fftw3.h>

#include <Eigen/Dense>

#include "MathematicsTypes.h"

namespace nsx {

class Convolver {

public:

    Convolver();

    Convolver(const Convolver& other)=default;

    Convolver& operator=(const Convolver& other)=default;

    virtual ~Convolver()=0;

    void reset();

    // Non-const getter for kernel parameter
    std::map<std::string,double>& parameters();

    // Const getter for kernel parameter
    const std::map<std::string,double>& parameters() const;

    void setParameters(const std::map<std::string,double>& parameters);

    RealMatrix matrix(int nrows, int ncols) const;

    //! Convolve an image
    RealMatrix convolve(const RealMatrix& image);

    virtual const char* name() const = 0;

protected:

    void updateKernel(int nrows, int ncols);

protected:

    virtual RealMatrix _matrix(int nrows, int cols) const=0;

    RealMatrix _kernel;

    std::map<std::string,double> _parameters;

    int _halfCols;

    // used directly with FFTW3
    fftw_plan _forwardPlan;

    fftw_plan _backwardPlan;

    double* _realData;

    fftw_complex* _transformedData;

    std::vector<std::complex<double>> _transformedKernel;
};

} // end namespace nsx
