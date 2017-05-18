/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
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

#ifndef NSXTOOL_TYPES_H_
#define NSXTOOL_TYPES_H_

#include <complex>
#include <map>
#include <memory>
#include <tuple>
#include <string>
#include <vector>

#include <Eigen/Core>

namespace nsx {

class Peak3D;
class UnitCell;
typedef std::shared_ptr<UnitCell> sptrUnitCell;
typedef std::shared_ptr<Peak3D> sptrPeak3D;
typedef std::vector<sptrUnitCell> CellList;

using isotopeContents=std::map<std::string,double>;

class Material;
using sptrMaterial = std::shared_ptr<Material>;

class Experiment;
typedef std::shared_ptr<Experiment> sptrExperiment;

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using ComplexMatrix = Eigen::Matrix<std::complex<double>, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

} // end namespace nsx

#endif /* NSXTOOL_TYPES_H_ */
