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
#include "base/utils/ParallelFor.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/src/Core/Matrix.h>

#include <iostream>
#include <mutex>

namespace ohkl {

QGrid2D::QGrid2D(sptrDataSet data, sptrUnitCell cell)
    : _data(data)
    , _cell(cell)
    , _q_min(1000, 1000, 1000)
    , _q_max(-1000, -1000, -1000)
    , _thread_parallel(true)
    , _max_threads(8)
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
    _grid_range = _q_max - _q_min;

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
    const std::size_t nframes = _data->nFrames();
    const std::size_t ncols = _data->nCols();
    const std::size_t nrows = _data->nRows();

    int z_idx = static_cast<int>(_fixed_direction);

    // Get the shape of the 2D grid, set the grid to zero
    std::vector<int> indices;
    for (std::size_t i = 0; i < 3; ++i) {
        if (i == z_idx)
            continue;
        indices.push_back(i);
    }
    int x_idx = indices.at(0);
    int y_idx = indices.at(1);
    double grid_freq = 1.0 / _grid_spacing;
    _grid = Eigen::MatrixXd::Zero(_ngridpoints[x_idx], _ngridpoints[y_idx]);

    std::cout << "x y z " << x_idx << " " << y_idx << " " << z_idx << std::endl;
    std::cout << _data->nCols() * _data->nRows() * _data->nFrames() << " Real space grid points"
              << std::endl;
    std::atomic_int bin_count = 0;
    std::mutex mut;
    parallel_for(nframes, [&](int start, int end) {
            for (int frame_idx = start; frame_idx < end; ++frame_idx) {
                Eigen::MatrixXi frame = _data->frame(frame_idx);
                Eigen::MatrixXd tmp_grid =
                    Eigen::MatrixXd::Zero(_ngridpoints[x_idx], _ngridpoints[y_idx]);

                for (std::size_t d_i = 0; d_i < ncols; ++d_i) {
                    for (std::size_t d_j = 0; d_j < nrows; ++d_j) {
                        const DetectorEvent event(d_i, d_j, frame_idx);
                        const ReciprocalVector qvec = _data->computeQ(event);
                        // Check that the pixel is in the slab
                        if (qvec[z_idx] < _grid_min[z_idx] || qvec[z_idx] > _grid_max[z_idx])
                            continue;

                        // Find the right q-space bin, add the count to it
                        const int count = frame(d_i, d_j);
                        const int grid_x_idx =
                            std::floor(std::fabs((qvec[x_idx] - _grid_min[x_idx]) / _grid_range[x_idx]) * _ngridpoints[x_idx]);
                        const int grid_y_idx =
                            std::floor((std::fabs(qvec[y_idx] - _grid_min[y_idx]) / _grid_range[y_idx]) * _ngridpoints[y_idx]);
                        // std::cout << grid_x_idx << " " << grid_y_idx << " " << count << std::endl;
                        if (grid_x_idx >= _ngridpoints[x_idx] || grid_y_idx >= _ngridpoints[y_idx])
                            continue;
                        tmp_grid(grid_x_idx, grid_y_idx) += count;
                        ++bin_count;
                    }
                }
                {
                    std::lock_guard<std::mutex> lock(mut);
                    _grid += tmp_grid;
                }
            }
        }, _thread_parallel, _max_threads);

    std::cout << bin_count << " real space bins mapped to q-space" << std::endl;
}

} // namespace ohkl
