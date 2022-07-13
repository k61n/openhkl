//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/SymOp.cpp
//! @brief     Implements class SymOp
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "tables/crystal/DoubleToFraction.h"
#include "tables/crystal/JonesSymbolParser.h"
#include "tables/crystal/SymOp.h"

namespace ohkl {

SymOp::SymOp(std::string generator)
{
    _matrix = parseJonesSymbol(generator);
}

SymOp::SymOp(const affineTransformation& symmetryOperation) : _matrix(symmetryOperation) { }

bool SymOp::operator==(const SymOp& other) const
{
    // If the rotation part of the symmetry operator of two
    // generators are not the same then the two generators are not equal.
    if (_matrix.linear() != other._matrix.linear())
        return false;

    // If the difference between the translation part of the symmetry
    // operator of two generators is not a vector of integers
    // then the two generators are not equal.
    auto deltat = _matrix.translation() - other._matrix.translation();

    return (
        (std::abs(std::remainder(deltat[0], 1.0)) <= 1.0e-9)
        && (std::abs(std::remainder(deltat[1], 1.0)) <= 1.0e-9)
        && (std::abs(std::remainder(deltat[2], 1.0)) <= 1.0e-9));
}

SymOp SymOp::operator*(const SymOp& other) const
{
    SymOp sym(_matrix * other._matrix);

    sym._matrix(0, 3) = std::remainder(sym._matrix(0, 3), 1.0);
    if (sym._matrix(0, 3) < 0)
        sym._matrix(0, 3) += 1.0;

    sym._matrix(1, 3) = std::remainder(sym._matrix(1, 3), 1.0);
    if (sym._matrix(1, 3) < 0)
        sym._matrix(1, 3) += 1.0;

    sym._matrix(2, 3) = std::remainder(sym._matrix(2, 3), 1.0);
    if (sym._matrix(2, 3) < 0)
        sym._matrix(2, 3) += 1.0;

    return sym;
}

const affineTransformation& SymOp::getMatrix() const
{
    return _matrix;
}

std::string SymOp::getJonesSymbol() const
{
    std::ostringstream os;
    char xyz[3] = {'x', 'y', 'z'};

    for (int i = 0; i < 3; ++i) {
        bool first(true);
        for (int j = 0; j < 3; ++j) {
            if (_matrix(i, j) == 0.0)
                continue;

            if (std::abs(_matrix(i, j) - 1.0) < 1.0e-3) {
                if (!first)
                    os << "+";
            } else if (std::abs(_matrix(i, j) + 1) < 1.0e-3)
                os << "-";
            else {
                if (!first && _matrix(i, j) > 0.0)
                    os << "+";
                os << _matrix(i, j);
            }
            os << xyz[j];
            first = false;
        }

        if (std::abs(_matrix(i, 3)) > 1.0e-3) {
            if (_matrix(i, 3) > 0)
                os << "+";
            long num, den;
            doubleToFraction(_matrix(i, 3), 100, num, den);
            os << num << "/" << den;
        }

        if (i < 2)
            os << ",";
    }

    return os.str();
}

int SymOp::getAxisOrder() const
{
    int order(0);
    int det = static_cast<int>(_matrix.linear().determinant());
    int trace = static_cast<int>(_matrix.linear().trace());

    switch (trace) {
        case (-3):
            if (det == -1)
                order = -1;
            break;
        case (-2):
            if (det == -1)
                order = -6;
            break;
        case (-1):
            if (det == -1)
                order = -4;
            if (det == 1)
                order = 2;
            break;
        case (0):
            if (det == -1)
                order = -3;
            if (det == 1)
                order = 3;
            break;
        case (1):
            if (det == -1)
                order = -2;
            if (det == 1)
                order = 4;
            break;
        case (2):
            if (det == 1)
                order = 6;
            break;
        case (3):
            if (det == 1)
                order = 1;
            break;
    }

    if (order == 0)
        throw std::runtime_error("Invalid axis order.");

    return order;
}

void SymOp::print(std::ostream& os) const
{
    os << getJonesSymbol();
}

std::ostream& operator<<(std::ostream& os, const SymOp& sym)
{
    sym.print(os);
    return os;
}

bool SymOp::hasTranslation() const
{
    return (
        std::abs(_matrix(0, 3)) > 1e-3 || std::abs(_matrix(1, 3)) > 1e-3
        || std::abs(_matrix(2, 3)) > 1e-3);
}

bool SymOp::isPureTranslation() const
{
    return (_matrix.linear().isIdentity(1.0e-3) && !_matrix.translation().isZero(1.0e-3));
}

Eigen::Vector3d SymOp::getTranslationPart() const
{
    return _matrix.translation();
}

Eigen::Matrix3d SymOp::getRotationPart() const
{
    return _matrix.linear();
}

int SymOp::translationIsIntegralMultiple(const SymOp& other) const
{
    const double epsilon = 1e-3;

    // return true only for non-zero multiple
    if (!other.hasTranslation() || !this->hasTranslation())
        return false;

    const Eigen::Vector3d myTranslation = this->getTranslationPart();
    const Eigen::Vector3d otherTranslation = other.getTranslationPart();

    int n = 0;
    for (unsigned int i = 0; i < 3; ++i) {
        if (std::abs(myTranslation[i]) > epsilon) {
            n = int(std::lround(otherTranslation[i] / myTranslation[i]));
            break;
        }
    }

    const Eigen::Vector3d difference = n * myTranslation - otherTranslation;
    const double norm = double(difference.adjoint() * difference);

    if (norm < epsilon)
        return n;
    return 0;
}

} // namespace ohkl
