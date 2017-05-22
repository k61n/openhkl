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

#ifndef NSXLIB_RESOLUTIONSHELL_H
#define NSXLIB_RESOLUTIONSHELL_H

#include <memory>
#include <set>
#include <vector>

#include <Eigen/Core>

namespace nsx {

class DataSet;
class Peak3D;

class ResolutionShell {
public:

    using sptrPeak3D = std::shared_ptr<Peak3D>;

    ResolutionShell(double dmin, double dmax, size_t num_shells);
    void addPeak(const sptrPeak3D& peak);
    const std::vector<std::vector<sptrPeak3D>>& getShells() const;
    const std::vector<double>& getD() const;
private:
    size_t _numShells;
    std::vector<std::vector<sptrPeak3D>> _shells;
    std::vector<double> _d;

};

} // end namespace nsx

#endif // NSXLIB_RESOLUTIONSHELL_H
