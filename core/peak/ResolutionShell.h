/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
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

#include <vector>

#include "PeakList.h"

namespace nsx {

//! \class DShell
//! \brief Utility structure to store a list of peaks with resolution in the range [dmin, dmax].
struct DShell {
    //! Minimum value of d, i.e. maximum resolution
    double dmin;
    //! Maximum value of d, i.e. minimum resolution
    double dmax;
    //! List of peaks contained within the shell
    PeakList peaks;
};

//! \class ResolutionShell
//! \brief Class to split a set of peaks into a number of shells based on resolution.
class ResolutionShell {
public:
    //! Construct the given number of cells with abolute minimum dmin and absolute maximum dmax.
    ResolutionShell(double dmin, double dmax, size_t num_shells);
    //! Add a peak to the list of shells. 
    //! It will automatically be added to the appropriate shell.
    void addPeak(const sptrPeak3D& peak);
    //! Return the given shell.
    const DShell& shell(size_t i) const;
    //! Return the number of shells.
    size_t nShells() const;

private:
    std::vector<DShell> _shells;
};

} // end namespace nsx
