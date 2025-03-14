//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/qspace/QGrid2D.h
//! @brief     Defines class QGrid2D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_QSPACE_QGRID2D_H
#define OHKL_CORE_QSPACE_QGRID2D_H

#include "core/data/DataTypes.h"

#include <Eigen/Dense>

#include <map>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace ohkl {

enum class Miller { h = 0, k = 1, l = 2 };

class QGrid2D {
 public:
    QGrid2D(sptrDataSet data, sptrUnitCell cell);

    //! Set the q-space and grid bounds given the Miller plane and q-spacing
    void initGrid(Miller fixed, double value, double qspacing, double thickness);
    //! Compute the bounding box of q-vectors for the data set
    void getQBounds();
    //! Generate an empty 2D grid along the chosen plane
    void getEmptyGrid();
    //! Populate the grid with intensities
    void sampleGrid();

    // Get a reference to the grid
    const RealMatrix& grid() { return _grid; };

 private:
    sptrDataSet _data; //!< pointer to the DataSet
    sptrUnitCell _cell; //!< pointer to the UnitCell
    Miller _fixed_direction; //!< element of (hkl) triplet to be fixed
    double _fixed_value; //!< value of (hkl) triplet element which is fixed
    double _grid_spacing; //!< q-space separation of grid points
    double _slab_thickness; //!< Width of slab to be integrated to get the grid
    RealMatrix _grid; //!< the 2D q-space grid
    Eigen::Vector3d _q_min; //!< Lower bound of q for the data set
    Eigen::Vector3d _q_max; //!< Upper bound of q for the data set
    Eigen::Vector3d _grid_min; //!< Lower bound of q for the 2D grid
    Eigen::Vector3d _grid_max; //!< Upper bound of q for the 2D grid
    Eigen::Vector3i _ngridpoints; //!< Number of grid points in each direction
};

} // namespace ohkl

#endif // OHKL_CORE_QSPACE_QGRID2D_H
