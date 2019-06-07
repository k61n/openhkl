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

#include <map>
#include <string>
#include <utility>

#include "MathematicsTypes.h"

namespace nsx {

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

} // end namespace nsx
