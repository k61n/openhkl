//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/NiggliReduction.cpp
//! @brief     Implements class NiggliReduction
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "tables/crystal/NiggliReduction.h"

namespace ohkl {

unsigned int NiggliReduction::_itermax = 10000;

NiggliReduction::NiggliReduction(const Eigen::Matrix3d& g, double epsilon)
    : _epsilon(epsilon), _g(g)
{
    if (epsilon <= 0 || epsilon > 1)
        throw std::runtime_error("Niggli reduction: epsilon must be in the range ]0,1]");

    // Multiply tolerance by approximate Unit-cell length
    _epsilon *= std::pow(g.determinant(), 1.0 / 6.0);
    updateParameters();
}

void NiggliReduction::setIterMax(unsigned int max)
{
    if (max == 0)
        throw std::runtime_error("Niggli reduction: max iterations must be  > 0");

    _itermax = max;
}

void NiggliReduction::reduce(Eigen::Matrix3d& newg, Eigen::Matrix3d& P)
{
    bool cond1, cond2, cond3;
    _P = Eigen::Matrix3d::Identity();
    updateParameters();

    for (unsigned int i = 0; i < _itermax; ++i) {
        // Starts the 8 conditional tests
        // Step 1
        if ((_A > _B + _epsilon)
            || (!(std::abs(_A - _B) > _epsilon) && (std::abs(_xi) > std::abs(_eta) + _epsilon))) {
            _CMat << 0, -1, 0, -1, 0, 0, 0, 0, -1;
            transformG();
            updateParameters();
        }
        // Step 2
        if (_B > _C + _epsilon
            || (!(std::abs(_B - _C) > _epsilon) && (std::abs(_eta) > std::abs(_zeta) + _epsilon))) {
            _CMat << -1, 0, 0, 0, 0, -1, 0, -1, 0;
            transformG();
            updateParameters();
            continue;
        }
        // Step 3
        int lmn = _l * _m * _n;
        if (lmn == 1) {
            int i = 1, j = 1, k = 1;

            if (_l == -1)
                i = -1;
            if (_m == -1)
                j = -1;
            if (_n == -1)
                k = -1;

            _CMat << i, 0, 0, 0, j, 0, 0, 0, k;
            transformG();
            updateParameters();
        }
        // Step 4
        else if (lmn == 0 || lmn == -1) {
            int i = 1, j = 1, k = 1;
            int* p = nullptr;
            if (_l == 1)
                i = -1;
            else if (_l == 0)
                p = &i;
            if (_m == 1)
                j = -1;
            else if (_l == 0)
                p = &j;
            if (_n == 1)
                k = -1;
            else if (_n == 0)
                p = &k;
            int ijk = i * j * k;
            if (ijk == -1) {
                if (p)
                    *p = -1;
            }
            _CMat << i, 0, 0, 0, j, 0, 0, 0, k;
            transformG();
            updateParameters();
        }
        // Step 5
        cond1 = std::abs(_xi) > _B + _epsilon;
        cond2 = !(std::abs(_B - _xi) > _epsilon) && ((2 * _eta) < (_zeta - _epsilon));
        cond3 = !(std::abs(_B + _xi) > _epsilon) && (_zeta < -_epsilon);

        if (cond1 || cond2 || cond3) {
            int sign = 0;
            if (_xi > 0)
                sign = 1;
            if (_xi < 0)
                sign = -1;
            _CMat << 1, 0, 0, 0, 1, -sign, 0, 0, 1;
            transformG();
            updateParameters();
            continue;
        }
        // Step 6
        cond1 = std::abs(_eta) > _A + _epsilon;
        cond2 = !(std::abs(_A - _eta) > _epsilon) && ((2 * _xi) < (_zeta - _epsilon));
        cond3 = !(std::abs(_A + _eta) > _epsilon) && (_zeta < -_epsilon);
        if (cond1 || cond2 || cond3) {
            int sign = 0;
            if (_eta > 0)
                sign = 1;
            if (_eta < 0)
                sign = -1;
            _CMat << 1, 0, -sign, 0, 1, 0, 0, 0, 1;
            transformG();
            updateParameters();
            continue;
        }
        // Step 7
        cond1 = std::abs(_zeta) > _A + _epsilon;
        cond2 = !(std::abs(_A - _zeta) > _epsilon) && ((2 * _xi) < (_eta - _epsilon));
        cond3 = !(std::abs(_A + _zeta) > _epsilon) && (_eta < -_epsilon);
        if (cond1 || cond2 || cond3) {
            int sign = 0;
            if (_zeta > 0)
                sign = 1;
            if (_zeta < 0)
                sign = -1;
            _CMat << 1, -sign, 0, 0, 1, 0, 0, 0, 1;
            transformG();
            updateParameters();
            continue;
        }
        // Step 8
        cond1 = (_xi + _eta + _zeta + _A + _B) < -_epsilon;
        cond2 = !(std::abs(_xi + _eta + _zeta + _A + _B) > _epsilon)
            && ((2 * (_A + _eta) + _zeta) > _epsilon);
        if (cond1 || cond2) {
            _CMat << 1, 0, 1, 0, 1, 1, 0, 0, 1;
            transformG();
            updateParameters();
            continue;
        }
        // If all 8 conditions are met, then cell is reduced.
        break;
    }
    newg = _g;
    P = _P;
}

void NiggliReduction::updateParameters()
{
    // Gets the Niggli parameters
    _A = _g(0, 0);
    _B = _g(1, 1);
    _C = _g(2, 2);
    _zeta = 2.0 * _g(0, 1);
    _eta = 2.0 * _g(0, 2);
    _xi = 2.0 * _g(1, 2);

    _l = 0;
    _m = 0;
    _n = 0;
    // Define the angles as acute or obtuse
    if (_xi < -_epsilon)
        _l = -1;
    else if (_xi > _epsilon)
        _l = 1;
    if (_eta < -_epsilon)
        _m = -1;
    else if (_eta > _epsilon)
        _m = 1;
    if (_zeta < -_epsilon)
        _n = -1;
    else if (_zeta > _epsilon)
        _n = 1;
}

void NiggliReduction::transformG()
{
    _P = _P * _CMat;
    // Transform to new tensor G'=(CMat^T). G. CMat
    _g = _CMat.transpose() * _g * _CMat;
}

} // namespace ohkl
