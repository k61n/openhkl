//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/GruberReduction.cpp
//! @brief     Implements class GruberReduction
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <stdexcept>

#include "tables/crystal/GruberReduction.h"

namespace nsx {

GruberReduction::GruberReduction(const Eigen::Matrix3d& g, double epsilon)
    : _g(g), _epsilon(epsilon)
{
    if (epsilon <= 0 || epsilon > 1)
        throw std::runtime_error("Gruber reduction: epsilon must be in the range ]0,1]");
    // Multiply tolerance by approximate squared Unit-cell length
    _epsilon *= std::pow(g.determinant(), 1.0 / 3.0);
}

int GruberReduction::reduce(Eigen::Matrix3d& P, LatticeCentring& centring, BravaisType& bravais)
{
    NiggliCharacter n = classify();
    P = n.P;

    switch (n.bravais[0]) {
        case 'a': bravais = BravaisType::Triclinic; break;
        case 'm': bravais = BravaisType::Monoclinic; break;
        case 'o': bravais = BravaisType::Orthorhombic; break;
        case 't': bravais = BravaisType::Tetragonal; break;
        case 'h': bravais = BravaisType::Hexagonal; break;
        case 'c': bravais = BravaisType::Cubic; break;
    }

    switch (n.bravais[1]) {
        case 'P': centring = LatticeCentring::P; break;
        case 'A': centring = LatticeCentring::A; break;
        case 'C': centring = LatticeCentring::C; break;
        case 'I': centring = LatticeCentring::I; break;
        case 'F': centring = LatticeCentring::F; break;
        case 'R': centring = LatticeCentring::R; break;
    }

    return n.number;
}

bool GruberReduction::equal(double A, double B) const
{
    return (std::fabs(A - B) < _epsilon);
}

NiggliCharacter GruberReduction::classify()
{
    const double A = _g(0, 0);
    const double B = _g(1, 1);
    const double C = _g(2, 2);
    const double D = 0.5 * (_g(1, 2) + _g(2, 1));
    const double E = 0.5 * (_g(0, 2) + _g(2, 0));
    const double F = 0.5 * (_g(0, 1) + _g(1, 0));

    const double s = std::signbit(D + E + F) ? -1.0 : 1.0;
    const double t = std::signbit(2 * D + F) ? -1.0 : 1.0;

    Eigen::VectorXd x(6);
    x << A, B, C, D, E, F;

    const bool typeI = [=]() -> bool {
        const double eps = std::sqrt(_epsilon);

        if (fabs(D) < eps)
            return false;
        if (fabs(E) < eps)
            return false;
        if (fabs(F) < eps)
            return false;
        return D * E * F > 0;
    }();

    NiggliCharacter n;

    for (auto i = 1; i <= 44; ++i) {
        n.set(i, s, t);

        if (typeI != n.typeI)
            continue;

        Eigen::VectorXd d = n.C * x;

        const double max = std::fabs(d.maxCoeff());
        const double min = std::fabs(d.minCoeff());

        const double score = std::max(max, min);

        // satisfied the conditions within tolerance
        if (score < _epsilon)
            return n;
    }

    // note: we should never reach this code
    throw std::runtime_error("could not classify Niggli character!");
}

bool NiggliCharacter::set(int priority, double s, double t)
{
    if (priority < 1 || priority > 44)
        return false;

    P.resize(3, 3);

    switch (priority) {
        case 1:
            number = 1;
            typeI = true;
            bravais = "cF";
            C.resize(5, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                1, 0, 0, -2, 0, 0, // D = A/2
                1, 0, 0, 0, -2, 0, // E = A/2
                1, 0, 0, 0, 0, -2; // F = A/2
            P << 1, -1, 1, 1, 1, -1, -1, 1, 1;
            P.transposeInPlace();
            return true;
        case 2:
            number = 2;
            typeI = true;
            bravais = "hR";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 1, -1, 0, // E = D
                0, 0, 0, 1, 0, -1; // F = D
            P << 1, -1, 0, -1, 0, 1, -1, -1, -1;
            P.transposeInPlace();
            return true;

        case 3:
            number = 3;
            typeI = false;
            bravais = "cP";
            C.resize(5, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0, // D = 0
                0, 0, 0, 0, 0, 1; // D = 0
            P << 1, 0, 0, 0, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 4:
            number = 5; // not a typo
            typeI = false;
            bravais = "cI";
            C.resize(5, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                1, 0, 0, 3, 0, 0, // D = -A/3
                1, 0, 0, 0, 3, 0, // D = -A/3
                1, 0, 0, 0, 0, 3; // D = -A/3
            P << 1, 0, 1, 1, 1, 0, 0, 1, 1;
            P.transposeInPlace();
            return true;

        case 5:
            number = 4; // not a typo
            typeI = false;
            bravais = "hR";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 1, -1, 0, // E = D
                0, 0, 0, 1, 0, -1; // F = D
            P << 1, -1, 0, -1, 0, 1, -1, -1, -1;
            P.transposeInPlace();
            return true;

        case 6:
            number = 6;
            typeI = false;
            bravais = "tI";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 1, -1, 0, // E = D
                1, 1, 0, -2 * s, -2 * s, -2 * s; // A+B = 2|D+E+F|
            P << 0, 1, 1, 1, 0, 1, 1, 1, 0;
            P.transposeInPlace();
            return true;

        case 7:
            number = 7;
            typeI = false;
            bravais = "tI";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 0, 1, -1, // F = E
                1, 1, 0, -2 * s, -2 * s, -2 * s; // A+B = 2|D+E+F|
            P << 1, 0, 1, 1, 1, 0, 0, 1, 1;
            P.transposeInPlace();
            return true;

        case 8:
            number = 8;
            typeI = false;
            bravais = "oI";
            C.resize(3, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 1, -1, 0, 0, 0, // B = C
                1, 1, 0, -2 * s, -2 * s, -2 * s; // A+B = 2|D+E+F|
            P << -1, -1, 0, -1, 0, -1, 0, -1, -1;
            P.transposeInPlace();
            return true;

        case 9:
            number = 9;
            typeI = true;
            bravais = "hR";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                1, 0, 0, -2, 0, 0, // D = A/2
                1, 0, 0, 0, -2, 0, // E = A/2
                1, 0, 0, 0, 0, -2; // F = A/2
            P << 1, 0, 0, -1, 1, 0, -1, -1, 3;
            P.transposeInPlace();
            return true;

        case 10:
            number = 10;
            typeI = true;
            bravais = "mC";
            C.resize(2, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 0, 0, 1, -1, 0; // E = D
            P << 1, 1, 0, 1, -1, 0, 0, 0, -1;
            P.transposeInPlace();
            return true;

        case 11:
            number = 11;
            typeI = false;
            bravais = "tP";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0, // E = 0
                0, 0, 0, 0, 0, 1; // F = 0
            P << 1, 0, 0, 0, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 12:
            number = 12;
            typeI = false;
            bravais = "hP";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0, // E = 0
                1, 0, 0, 0, 0, 2; // F = -A/2
            P << 1, 0, 0, 0, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 13:
            number = 13;
            typeI = false;
            bravais = "oC";
            C.resize(3, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0; // E = 0
            P << 1, 1, 0, -1, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 14:
            number = 15; // not a typo
            typeI = false;
            bravais = "tI";
            C.resize(4, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                1, 0, 0, 2, 0, 0, // D = -A/2
                1, 0, 0, 0, 2, 0, // E = -A/2
                0, 0, 0, 0, 0, 1; // F = 0
            P << 1, 0, 0, 0, 1, 0, 1, 1, 2;
            P.transposeInPlace();
            return true;

        case 15:
            number = 16; // not a typo
            typeI = false;
            bravais = "oF";
            C.resize(3, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 0, 0, 1, -1, 0, // E = D
                1, 1, 0, -2 * s, -2 * s, -2 * s; // A+B = 2|D+E+F|
            P << -1, -1, 0, 1, -1, 0, 1, 1, 2;
            P.transposeInPlace();
            return true;

        case 16:
            number = 14; // not a typo
            typeI = false;
            bravais = "mC";
            C.resize(2, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                0, 0, 0, 1, -1, 0; // E = D
            P << 1, 1, 0, -1, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 17:
            number = 17;
            typeI = false;
            bravais = "mC";
            C.resize(2, 6);
            C << 1, -1, 0, 0, 0, 0, // A = B
                1, 1, 0, -2 * s, -2 * s, -2 * s; // A+B = 2|D+E+F|
            P << 1, -1, 0, 1, 1, 0, -1, 0, -1;
            P.transposeInPlace();
            return true;

        case 18:
            number = 18;
            typeI = true;
            bravais = "tI";
            C.resize(4, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                1, 0, 0, -4, 0, 0, // D = A/4
                1, 0, 0, 0, -2, 0, // E = A/2
                1, 0, 0, 0, 0, -2; // F = A/2
            P << 0, -1, 1, 1, -1, -1, 1, 0, 0;
            P.transposeInPlace();
            return true;

        case 19:
            number = 19;
            typeI = true;
            bravais = "oI";
            C.resize(3, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                1, 0, 0, 0, -2, 0, // E = A/2
                1, 0, 0, 0, 0, -2; // F = A/2
            P << -1, 0, 0, 0, -1, 1, -1, 1, 1;
            P.transposeInPlace();
            return true;

        case 20:
            number = 20;
            typeI = true;
            bravais = "mC";
            C.resize(2, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 0, 1, -1; // F = E
            P << 0, 1, 1, 0, 1, -1, -1, 0, 0;
            P.transposeInPlace();
            return true;

        case 21:
            number = 21;
            typeI = false;
            bravais = "tP";
            C.resize(4, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0, // E = 0
                0, 0, 0, 0, 0, 0; // F = 0
            P << 0, 1, 0, 0, 0, 1, 1, 0, 0;
            P.transposeInPlace();
            return true;

        case 22:
            number = 22;
            typeI = false;
            bravais = "hP";
            C.resize(2, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                0, 1, 0, 2, 0, 0; // D = -B/2
            P << 0, 1, 0, 0, 0, 1, 1, 0, 0;
            P.transposeInPlace();
            return true;

        case 23:
            number = 23;
            typeI = false;
            bravais = "oC";
            C.resize(3, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 0, 1, 0, // E = 0
                0, 0, 0, 0, 0, 1; // F = 0
            P << 0, 1, 1, 0, -1, 1, 1, 0, 0;
            P.transposeInPlace();
            return true;

        case 24:
            number = 24;
            typeI = false;
            bravais = "hR";
            C.resize(4, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                1, 0, 0, 0, 3, 0, // E = -A/3
                1, 0, 0, 0, 0, 3, // F = -A/3
                1, 1, 0, -2 * s, -2 * s, -2 * s; // A+B = 2|D+E+F|
            P << 1, 2, 1, 0, -1, 1, 1, 0, 0;
            P.transposeInPlace();
            return true;

        case 25:
            number = 25;
            typeI = false;
            bravais = "mC";
            C.resize(2, 6);
            C << 0, 1, -1, 0, 0, 0, // B = C
                0, 0, 0, 0, 1, -1; // E = F
            P << 0, 1, 1, 0, -1, 1, 1, 0, 0;
            P.transposeInPlace();
            return true;

        case 26:
            number = 26;
            typeI = true;
            bravais = "oF";
            C.resize(3, 6);
            C << 1, 0, 0, -4, 0, 0, // D = A/4
                1, 0, 0, 0, -2, 0, // E = A/2
                1, 0, 0, 0, 0, -2; // F = A/2
            P << 1, 0, 0, -1, 2, 0, -1, 0, 2;
            P.transposeInPlace();
            return true;

        case 27:
            number = 27;
            typeI = true;
            bravais = "mC";
            C.resize(2, 6);
            C << 1, 0, 0, 0, -2, 0, // E = A/2
                1, 0, 0, 0, 0, -2; // F = A/2
            P << -1, 2, 0, -1, 0, 0, 0, -1, 1;
            P.transposeInPlace();
            return true;

        case 28:
            number = 28;
            typeI = true;
            bravais = "mC";
            C.resize(2, 6);
            C << 1, 0, 0, 0, -2, 0, // E = A/2
                0, 0, 0, 2, 0, -1; // F = 2D
            P << -1, 0, 0, -1, 0, 2, 0, 1, 0;
            P.transposeInPlace();
            return true;

        case 29:
            number = 29;
            typeI = true;
            bravais = "mC";
            C.resize(2, 6);
            C << 0, 0, 0, 2, -1, 0, // E = 2D
                1, 0, 0, 0, 0, -2; // F = A/2
            P << 1, 0, 0, 1, -2, 0, 0, 0, -1;
            P.transposeInPlace();
            return true;

        case 30:
            number = 30;
            typeI = true;
            bravais = "mC";
            C.resize(2, 6);
            C << 0, 1, 0, -2, 0, 0, // D = B/2
                0, 0, 0, 0, 2, -1; // F = 2E
            P << 0, 1, 0, 0, 1, -2, -1, 0, 0;
            P.transposeInPlace();
            return true;

        case 31:
            number = 31;
            typeI = true;
            bravais = "aP";
            C.resize(1, 6);
            C << 0, 0, 0, 0, 0, 0;
            P << 1, 0, 0, 0, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 32:
            number = 32;
            typeI = false;
            bravais = "oP";
            C.resize(3, 6);
            C << 0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0, // E = 0
                0, 0, 0, 0, 0, 1; // F = 0
            P << 1, 0, 0, 0, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 33:
            number = 40; // not a typo
            typeI = false;
            bravais = "oC";
            C.resize(3, 6);
            C << 0, 1, 0, 2, 0, 0, // D = -B/2
                0, 0, 0, 0, 1, 0, // E = 0
                0, 0, 0, 0, 0, 1; // F = 0
            P << 0, -1, 0, 0, 1, 2, -1, 0, 0;
            P.transposeInPlace();
            return true;

        case 34:
            number = 35; // not a typo
            typeI = false;
            bravais = "mP";
            C.resize(2, 6);
            C << 0, 0, 0, 0, 1, 0, // E = 0
                0, 0, 0, 0, 0, 1; // F = 0
            P << 0, -1, 0, -1, 0, 0, 0, 0, -1;
            P.transposeInPlace();
            return true;

        case 35:
            number = 36; // not a typo
            typeI = false;
            bravais = "oC";
            C.resize(3, 6);
            C << 0, 0, 0, 1, 0, 0, // D = 0
                1, 0, 0, 0, 2, 0, // E = -A/2
                0, 0, 0, 0, 0, 1, // F = 0
                P << 1, 0, 0, -1, 0, -2, 0, 1, 0;
            P.transposeInPlace();
            return true;

        case 36:
            number = 33; // not a typo
            typeI = false;
            bravais = "mP";
            C.resize(2, 6);
            C << 0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 0, 1; // F = 0
            P << 1, 0, 0, 0, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;

        case 37:
            number = 38; // not a typo
            typeI = false;
            bravais = "oC";
            C.resize(3, 6);
            C << 0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0, // E = 0
                1, 0, 0, 0, 0, 2; // F = -A/2
            P << -1, 0, 0, 1, 2, 0, 0, 0, -1;
            P.transposeInPlace();
            return true;

        case 38:
            number = 34; // not a typo
            typeI = false;
            bravais = "mP";
            C.resize(2, 6);
            C << 0, 0, 0, 1, 0, 0, // D = 0
                0, 0, 0, 0, 1, 0; // E = 0
            P << -1, 0, 0, 0, 0, -1, 0, -1, 0;
            P.transposeInPlace();
            return true;

        case 39:
            number = 42; // not a typo
            typeI = false;
            bravais = "oI";
            C.resize(3, 6);
            C << 0, 1, 0, 2, 0, 0, // D = -B/2
                1, 0, 0, 0, 2, 0, // E = -A/2
                0, 0, 0, 0, 0, 1; // F = 0
            P << -1, 0, 0, 0, -1, 0, 1, 1, 2;
            P.transposeInPlace();
            return true;

        case 40:
            number = 41; // not a typo
            typeI = false;
            bravais = "mC";
            C.resize(2, 6);
            C << 0, 1, 0, 2, 0, 0, // D = -B/2
                0, 0, 0, 0, 0, 1; // F = 0
            P << 0, -1, -2, 0, -1, 0, -1, 0, 0;
            P.transposeInPlace();
            return true;

        case 41:
            number = 37; // not a typo
            typeI = false;
            bravais = "mC";
            C.resize(2, 6);
            C << 1, 0, 0, 0, 2, 0, // E = -A/2
                0, 0, 0, 0, 0, 1; // F = 0
            P << 1, 0, 2, 1, 0, 0, 0, 1, 0;
            P.transposeInPlace();
            return true;

        case 42:
            number = 39; // not a typo
            typeI = false;
            bravais = "mC";
            C.resize(2, 6);
            C << 0, 0, 0, 0, 1, 0, // E = 0
                1, 0, 0, 0, 0, 2; // F = -A/2
            P << -1, -2, 0, -1, 0, 0, 0, 0, -1;
            P.transposeInPlace();
            return true;

        case 43:
            number = 43;
            typeI = false;
            bravais = "mI";
            C.resize(2, 6);
            C << 1, 1, 0, -2 * s, -2 * s, -2 * s, // A+B = 2|D+E+F|
                0, 1, 0, -2 * t, 0, -t; // B = |2*D+F|
            P << -1, 0, 0, -1, -1, -2, 0, -1, 0;
            P.transposeInPlace();
            return true;

        case 44:
            number = 44; // not a typo
            typeI = false;
            bravais = "aP";
            C.resize(1, 6);
            C << 0, 0, 0, 0, 0, 0;
            P << 1, 0, 0, 0, 1, 0, 0, 0, 1;
            P.transposeInPlace();
            return true;
    }
    return false;
}

} // namespace nsx
