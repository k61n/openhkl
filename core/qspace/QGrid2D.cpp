//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/qspace/QGrid2D.cpp
//! @brief     Implements class QGrid2D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/qspace/QGrid2D.h"

#include "base/geometry/DirectVector.h"
#include "base/geometry/ReciprocalVector.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/src/Core/Matrix.h>
#include <iostream>

namespace ohkl {

QGrid2D::QGrid2D(sptrDataSet data, sptrUnitCell cell)
    : _data(data)
    , _cell(cell)
    , _q_min(1000, 1000, 1000)
    , _q_max(-1000, -1000, -1000)
{
}

void QGrid2D::initGrid(Miller fixed, double value, double qspacing, double thickness)
{
    _fixed_direction = fixed;
    _fixed_value = value;
    _grid_spacing = qspacing;
    _slab_thickness = thickness;

    getQBounds();
    getEmptyGrid();
}

void QGrid2D::getQBounds()
{
    DetectorEvent max(_data->nRows() - 1, _data->nCols() - 1, _data->nFrames() - 1);
    DetectorEvent min(0, 0, 0);

    DetectorEvent d000 = min;
    DetectorEvent dx00(max.px, 0, 0);
    DetectorEvent d0y0(0, max.py, 0);
    DetectorEvent d00z(0, 0, max.frame);
    DetectorEvent dxy0(max.px, max.py, 0);
    DetectorEvent dx0z(max.px, 0, max.frame);
    DetectorEvent d0yz(0, max.py, max.frame);
    DetectorEvent dxyz = max;

    std::vector<ReciprocalVector> qs;
    qs.push_back(_data->computeQ(d000));
    qs.push_back(_data->computeQ(dx00));
    qs.push_back(_data->computeQ(d0y0));
    qs.push_back(_data->computeQ(d00z));
    qs.push_back(_data->computeQ(dxy0));
    qs.push_back(_data->computeQ(dx0z));
    qs.push_back(_data->computeQ(d0yz));
    qs.push_back(_data->computeQ(dxyz));

    for (const auto& q : qs) {
        _q_min[0] = std::min(_q_min[0], q[0]);
        _q_min[1] = std::min(_q_min[1], q[1]);
        _q_min[2] = std::min(_q_min[2], q[2]);
        _q_max[0] = std::max(_q_max[0], q[0]);
        _q_max[1] = std::max(_q_max[1], q[1]);
        _q_max[2] = std::max(_q_max[2], q[2]);
    }
}

void QGrid2D::getEmptyGrid()
{
    Eigen::RowVector3d intersecting_peak = {0, 0, 0};
    int idx = static_cast<int>(_fixed_direction);
    intersecting_peak[idx] = _fixed_value;

    _grid_min = _q_min;
    _grid_max = _q_max;

    Eigen::RowVector3d origin_q = _cell->fromIndex(intersecting_peak);
    for (std::size_t i = 0; i < 3; ++i) {
        if (i == idx) {
            _grid_min[i] = origin_q[i] - _slab_thickness;
            _grid_max[i] = origin_q[i] + _slab_thickness;
            _ngridpoints[i] = 1;
        } else {
            _ngridpoints[i] = std::ceil((_grid_max[i] - _grid_min[i]) / _grid_spacing);
        }
    }

    std::cout << _grid_min.transpose() << std::endl;
    std::cout << _grid_max.transpose() << std::endl;
    std::cout << _ngridpoints.transpose() << std::endl;
}

void QGrid2D::sampleGrid()
{
    int z_idx = static_cast<int>(_fixed_direction);

    // Get the shape of the 2D grid, set the grid to zero
    std::vector<int> indices;
    for (std::size_t i = 0; i < 3; ++i) {
        if (i == z_idx)
            continue;
        indices.push_back(i);
    }
    _grid = Eigen::MatrixXd::Zero(_ngridpoints[indices.at(0)], _ngridpoints[indices.at(1)]);

    std::vector<int> grid_idx;
    for (std::size_t frame_idx = 0; frame_idx < _data->nFrames(); ++frame_idx) {
        Eigen::MatrixXi frame = _data->frame(frame_idx);
        for (std::size_t d_i = 0; d_i < _data->nCols(); ++d_i) {
            for (std::size_t d_j = 0; d_j < _data->nRows(); ++d_j) {
                DetectorEvent event(d_i, d_j, frame_idx);
                ReciprocalVector q = _data->computeQ(event);
                // Check that the pixel is in the slab
                if (q[z_idx] < _grid_min[z_idx] || q[z_idx] > _grid_max[z_idx])
                    continue;

                // Find the right q-space bin, add the count to it
                int count = frame(d_i, d_j);
                grid_idx.clear();
                for (std::size_t q_idx = 0; q_idx < 3; ++q_idx) {
                    if (q_idx == z_idx)
                        continue;
                    grid_idx.push_back(
                        (int)(q[q_idx] - _grid_min[q_idx]) / (_grid_max[q_idx] - _grid_min[q_idx]));
                }
                _grid(grid_idx[0], grid_idx[1]) += count;
            }
        }
    }
}

} // namespace ohkl
