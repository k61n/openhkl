#include "GruberReduction.h"

#include <cmath>
#include <stdexcept>

namespace nsx
{
namespace Crystal
{

GruberReduction::GruberReduction(const Eigen::Matrix3d& g, double epsilon):
    _g(g),
    _epsilon(epsilon)
{
    if (epsilon<=0 || epsilon>1) {
        throw std::runtime_error("Gruber reduction: epsilon must be in the range ]0,1]");
    }
    // Multiply tolerance by approximate squared Unit-cell length
    _epsilon *= std::pow(g.determinant(), 1.0/3.0);
}

int GruberReduction::reduce(Eigen::Matrix3d& P,LatticeCentring& centring, BravaisType& bravais)
{
    const double A=_g(0,0);
    const double B=_g(1,1);
    const double C=_g(2,2);
    const double D=_g(1,2);
    const double E=_g(0,2);
    const double F=_g(0,1);
    const double eps = std::sqrt(_epsilon);
    const bool typeI = [=]() -> bool {
        if ( fabs(D) < eps) {
            return false;
        }
        if ( fabs(E) < eps) {
            return false;
        }
        if ( fabs(F) < eps) {
            return false;
        }
        return D*E*F > 0;
    }();

    // A = B = C
    if (equal(A,B) && equal(B,C)) {
        // Condition 1
        if (typeI && equal(D,0.5*A) && equal(E,0.5*A) && equal(F,0.5*A)) {
            centring=LatticeCentring::F;
            bravais=BravaisType::Cubic;
            P << 1, 1,-1,
                -1, 1, 1,
                 1,-1, 1;
            return 1;
        }
        // Condition 2
        if (typeI && equal(E,D) && equal(F,D)) {
            centring=LatticeCentring::R;
            bravais=BravaisType::Hexagonal;
            P << 1,-1,-1,
                -1, 0,-1,
                 0, 1,-1;
            return 2;
        }
        // Condition 3
        if (!typeI && equal(D,0) && equal(E,0) && equal(F,0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Cubic;
            P << 1, 0, 0,
                 0, 1, 0,
                 0, 0, 1;
            return 3;
        }
        // Condition 5
        if (!typeI && equal(D,-A/3) && equal(E,-A/3) && equal(F,-A/3)) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Cubic;
            P << 1, 1, 0,
                 0, 1, 1,
                 1, 0, 1;
            return 5;
        }
        // Condition 4
        if (!typeI && equal(D,E) && equal(D,F)) {
            centring=LatticeCentring::R;
            bravais=BravaisType::Hexagonal;
            P << 1,-1,-1,
                -1, 0,-1,
                 0, 1,-1;
            return 4;
        }
        // Condition 6
        if (!typeI && equal(D,E) && equal(A+B,2.0*std::fabs(D+E+F))) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Tetragonal;
            P << 0, 1, 1,
                 1, 0, 1,
                 1, 1, 0;
            return 6;
        }
        // Condition 7
        if (!typeI && equal(E,F) && equal(A+B,2.0*std::fabs(D+E+F))) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Tetragonal;
            P << 1, 1, 0,
                 0, 1, 1,
                 1, 0, 1;
            return 7;
        }
        // Conditoin 8
        if (!typeI && equal(A+B,2.0*std::fabs(D+E+F))) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Orthorhombic;
            P << -1,-1, 0,
                 -1, 0,-1,
                  0,-1,-1;
            return 8;
        }
        throw std::runtime_error("A = B = C but could not classify cell type");
    }
    else if (equal(A,B)) {
        // Condition 9
        if (typeI && equal(D,A/2) && equal(E,A/2) && equal(F,A/2)) {
            centring=LatticeCentring::R;
            bravais=BravaisType::Hexagonal;
            P <<  1,-1,-1,
                  0, 1,-1,
                  0, 0, 3;
            return 9;
        }
        // Condition 10
        if (typeI && equal(D,E)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  1, 1, 0,
                  1,-1, 0,
                  0, 0,-1;
            return 10;
        }
        // Condition 11
        if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Tetragonal;
            P <<  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1;
            return 11;
        }
        // Condition 12
        if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,-A/2)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Hexagonal;
            P <<  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1;
            return 12;
        }
        // Condition 13
        if (!typeI && equal(D,0.0) && equal(E,0.0)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Orthorhombic;
            P <<  1,-1, 0,
                  1, 1, 0,
                  0, 0, 1;
            return 13;
        }
        // Condition 15
        if (!typeI && equal(D,-A/2) && equal(E,-A/2) && equal(F,0.0)) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Tetragonal;
            P <<  1, 0, 1,
                  0, 1, 1,
                  0, 0, 2;
            return 15;
        }
        // Condition 16
        if (!typeI && equal(D,E) && equal(A+B,2.0*std::fabs(D+E+F))) {
            centring=LatticeCentring::F;
            bravais=BravaisType::Orthorhombic;
            P << -1, 1, 1,
                 -1,-1, 1,
                  0, 0, 2;
            return 16;
        }
        // Condition 14
        if (!typeI && equal(D,E)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  1,-1, 0,
                  1, 1, 0,
                  0, 0, 1;
            return 14;
        }
        // Condition 17
        if (!typeI && equal(A+B,2.0*std::fabs(D+E+F))) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  1, 1,-1,
                 -1, 1, 0,
                  0, 0,-1;
            return 17;
        }
        throw std::runtime_error("A = B but could not classify cell type");
    }
    else if (equal(B,C)) {
        // Condition 18
        if (typeI && equal(D,A/4) && equal(E,A/2) && equal(F,A/2)) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Tetragonal;
            P <<  0, 1, 1,
                 -1,-1, 0,
                  1,-1, 0;
            return 18;
        }
        // Condition 19
        if (typeI && equal(E,A/2) && equal(F,A/2)) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Orthorhombic;
            P << -1, 0,-1,
                  0,-1, 1,
                  0, 1, 1;
            return 19;
        }
        // Condition 20
        if (typeI && equal(E,F)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  0, 0,-1,
                  1, 1, 0,
                  1,-1, 0;
            return 20;
        }
        // Condition 21
        if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Tetragonal;
            P <<  0, 0, 1,
                  1, 0, 0,
                  0, 1, 0;
            return 21;
        }
        // Condition 22
        if (!typeI && equal(D,-B/2) && equal(E,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Hexagonal;
            P <<  0, 0, 1,
                  1, 0, 0,
                  0, 1, 0;
            return 22;
        }
        // Condition 23
        if (!typeI && equal(E,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Orthorhombic;
            P <<  0, 0, 1,
                  1,-1, 0,
                  1, 1, 0;
            return 23;
        }
        // Condition 24
        if (!typeI && equal(A+B,2.0*std::fabs(D+E+F)) && equal(E,-A/3) && equal(F,-A/3)) {
            centring=LatticeCentring::R;
            bravais=BravaisType::Hexagonal;
            P <<  1, 0, 1,
                  2,-1, 0,
                  1, 1, 0;
            return 24;
        }
        // Condition 25
        if (!typeI && equal(E,F)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  0, 0, 1,
                  1,-1, 0,
                  1, 1, 0;
            return 25;
        }
        throw std::runtime_error("B = C but could not classify cell type");
    }
    else {
        // Condition 26
        if (typeI && equal(D,A/4) && equal(E,A/2) && equal(F,A/2)) {
            centring=LatticeCentring::F;
            bravais=BravaisType::Orthorhombic;
            P <<  1,-1,-1,
                  0, 2, 0,
                  0, 0, 2;
            return 26;
        }
        // Condition 27
        if (typeI && equal(E,A/2) && equal(F,A/2)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P << -1,-1, 0,
                  2, 0,-1,
                  0, 0, 1;
            return 27;
        }
        // Condition 28
        if (typeI && equal(E,A/2) && equal(F,2*D)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P << -1,-1, 0,
                  0, 0, 1,
                  0, 2, 0;
            return 28;
        }
        // Condition 29
        if (typeI && equal(E,2*D) && equal(F,A/2)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  1, 1, 0,
                  0,-2, 0,
                  0, 0,-1;
            return 29;
        }
        // Condition 30
        if (typeI && equal(D,B/2) && equal(F,2*E)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  0, 0,-1,
                  1, 1, 0,
                  0,-2, 0;
            return 30;
        }
        // Condition 31
        if (typeI) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Triclinic;
            P <<  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1;
            return 31;
        }
        // Condition 32
        if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Orthorhombic;
            P <<  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1;
            return 32;
        }
        // Condition 40
        if (!typeI && equal(D,-B/2) && equal(E,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Orthorhombic;
            P <<  0, 0,-1,
                 -1, 1, 0,
                  0, 2, 0;
            return 40;
        }
        // Condition 35
        if (!typeI && equal(E,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Monoclinic;
            P <<  0,-1, 0,
                 -1, 0, 0,
                  0, 0,-1;
            return 35;
        }
        // Condition 36
        if (!typeI && equal(D,0.0) && equal(E,-A/2) && equal(F,0.0)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Orthorhombic;
            P <<  1,-1, 0,
                  0, 0, 1,
                  0,-2, 0;
            return 36;
        }
        // Condition 33
        if (!typeI && equal(D,0.0) && equal(F,0.0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Monoclinic;
            P <<  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1;
            return 33;
        }
        // Condition 38
        if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,-A/2)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Orthorhombic;
            P << -1, 1, 0,
                  0, 2, 0,
                  0, 0,-1;
            return 38;
        }
        // Condition 34
        if (!typeI && equal(D,0.0) && equal(E,0.0)) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Monoclinic;
            P << -1, 0, 0,
                  0, 0,-1,
                  0,-1, 0;
            return 34;
        }
        // Condition 42
        if (!typeI && equal(D,-B/2) && equal(E,-A/2) && equal(F,0.0)) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Orthorhombic;
            P << -1, 0, 1,
                  0,-1, 1,
                  0, 0, 2;
            return 42;
        }
        // Condition 41
        if (!typeI && equal(D,-B/2) && equal(F,0.0)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  0, 0,-1,
                 -1,-1, 0,
                 -2, 0, 0;
            return 41;
        }
        // Condition 37
        if (!typeI && equal(E,-A/2) && equal(F,0.0)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P <<  1, 1, 0,
                  0, 0, 1,
                  2, 0, 0;
            return 37;
        }
        // Condition 39
        if (!typeI && equal(E,0.0) && equal(F,-A/2)) {
            centring=LatticeCentring::C;
            bravais=BravaisType::Monoclinic;
            P << -1,-1, 0,
                 -2, 0, 0,
                  0, 0,-1;
            return 39;
        }
        // Condition 43
        if (!typeI && equal(A+B,2.0*std::fabs(D+E+F)) && equal(B,std::fabs(2*D+F))) {
            centring=LatticeCentring::I;
            bravais=BravaisType::Monoclinic;
            P << -1,-1, 0,
                  0,-1,-1,
                  0,-2, 0;
            return 43;
        }
        // Condition 44
        if (!typeI) {
            centring=LatticeCentring::P;
            bravais=BravaisType::Triclinic;
            P <<  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1;
            return 44;
        }
        throw std::runtime_error("A != B != C but could not classify cell type");
    }
    throw std::runtime_error(std::string("failed to classify cell into one of the 44 conditions!"));
}

bool GruberReduction::equal(double A, double B) const
{
    return (std::fabs(A-B)<_epsilon);
}

} // namespace Crystal
} // namespace nsx
