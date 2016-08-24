#include "GruberReduction.h"

#include <cmath>
#include <stdexcept>

namespace SX
{
namespace Crystal
{

GruberReduction::GruberReduction(const Eigen::Matrix3d& g, double epsilon):
    _g(g),
    _epsilon(epsilon)
{
	if (epsilon<=0 || epsilon>1)
		throw std::runtime_error("Gruber reduction: epsilon must be in the range ]0,1]");

    // Multiply tolerance by approximate squared Unit-cell length
    _epsilon *= std::pow(g.determinant(), 1.0/3.0);
}

int GruberReduction::reduce(Eigen::Matrix3d& P,LatticeCentring& centring,BravaisType& bravais)
{
	double A=_g(0,0);
	double B=_g(1,1);
	double C=_g(2,2);
	double D=_g(1,2);
	double E=_g(0,2);
	double F=_g(0,1);

    double eps = std::sqrt(_epsilon);

    bool typeI;

    if ( fabs(D) < eps)
        typeI = false;
    else if ( fabs(E) < eps)
        typeI = false;
    else if ( fabs(F) < eps)
        typeI = false;
    else
        typeI = D*E*F > 0;

	if (equal(A,B) && equal(B,C)) //A=B=C
	{
		if (typeI && equal(D,0.5*A) && equal(E,0.5*A) && equal(F,0.5*A)) // Condition 1
		{
			centring=LatticeCentring::F;
			bravais=BravaisType::Cubic;
			P << 1, 1,-1,
				-1, 1, 1,
				 1,-1, 1;
            return 1;
		}
		if (typeI && equal(E,D) && equal(F,D)) // Condition 2
		{
			centring=LatticeCentring::R;
			bravais=BravaisType::Hexagonal;
			P << 1,-1,-1,
				-1, 0,-1,
				 0, 1,-1;
            return 2;
		}
		if (!typeI && equal(D,0) && equal(E,0) && equal(F,0)) // Condition 3
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Cubic;
			P << 1, 0, 0,
				 0, 1, 0,
				 0, 0, 1;
            return 3;
		}
		if (!typeI && equal(D,-A/3) && equal(E,-A/3) && equal(F,-A/3)) // Condition 5
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Cubic;
			P << 1, 1, 0,
				 0, 1, 1,
				 1, 0, 1;
            return 5;
		}
		if (!typeI && equal(D,E) && equal(D,F)) // Condition 4
		{
			centring=LatticeCentring::R;
			bravais=BravaisType::Hexagonal;
			P << 1,-1,-1,
				-1, 0,-1,
				 0, 1,-1;
            return 4;
		}
		if (!typeI && equal(D,E) && equal(A+B,2.0*std::fabs(D+E+F))) // Condition 6
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Tetragonal;
			P << 0, 1, 1,
				 1, 0, 1,
				 1, 1, 0;
            return 6;
		}
		if (!typeI && equal(E,F) && equal(A+B,2.0*std::fabs(D+E+F))) // Condition 7
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Tetragonal;
			P << 1, 1, 0,
				 0, 1, 1,
				 1, 0, 1;
            return 7;
		}
		if (!typeI && equal(A+B,2.0*std::fabs(D+E+F))) // Condition 8
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Orthorhombic;
			P << -1,-1, 0,
				 -1, 0,-1,
				  0,-1,-1;
            return 8;
		}

        throw std::runtime_error("A=B=C but could not classify cell type");
	}
	else if (equal(A,B))
	{
		if (typeI && equal(D,A/2) && equal(E,A/2) && equal(F,A/2)) // Condition 9
		{
			centring=LatticeCentring::R;
			bravais=BravaisType::Hexagonal;
			P <<  1,-1,-1,
				  0, 1,-1,
				  0, 0, 3;
            return 9;
		}
		if (typeI && equal(D,E)) // Condition 10
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  1, 1, 0,
				  1,-1, 0,
				  0, 0,-1;
            return 10;
		}
		if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,0.0)) // Condition 11
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Tetragonal;
			P <<  1, 0, 0,
				  0, 1, 0,
				  0, 0, 1;
            return 11;
		}
		if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,-A/2)) // Condition 12
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Hexagonal;
			P <<  1, 0, 0,
				  0, 1, 0,
				  0, 0, 1;
            return 12;
		}
		if (!typeI && equal(D,0.0) && equal(E,0.0)) // Condition 13
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Orthorhombic;
			P <<  1,-1, 0,
				  1, 1, 0,
				  0, 0, 1;
            return 13;
		}
		if (!typeI && equal(D,-A/2) && equal(E,-A/2) && equal(F,0.0)) // Condition 15
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Tetragonal;
			P <<  1, 0, 1,
				  0, 1, 1,
				  0, 0, 2;
            return 15;
		}
		if (!typeI && equal(D,E) && equal(A+B,2.0*std::fabs(D+E+F))) // Condition 16
		{
			centring=LatticeCentring::F;
			bravais=BravaisType::Orthorhombic;
			P << -1, 1, 1,
				 -1,-1, 1,
				  0, 0, 2;
            return 16;
		}
		if (!typeI && equal(D,E)) // Condition 14
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  1,-1, 0,
				  1, 1, 0,
				  0, 0, 1;
            return 14;
		}
		if (!typeI && equal(A+B,2.0*std::fabs(D+E+F))) // Condition 17
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  1, 1,-1,
				 -1, 1, 0,
				  0, 0,-1;
            return 17;
		}

        throw std::runtime_error("A=B but could not classify cell type");
	}
	else if (equal(B,C))
	{
		if (typeI && equal(D,A/4) && equal(E,A/2) && equal(F,A/2)) // Condition 18
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Tetragonal;
			P <<  0, 1, 1,
				 -1,-1, 0,
				  1,-1, 0;
            return 18;
		}
		if (typeI && equal(E,A/2) && equal(F,A/2)) // Condition 19
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Orthorhombic;
			P << -1, 0,-1,
				  0,-1, 1,
				  0, 1, 1;
            return 19;
		}
		if (typeI && equal(E,F)) // Condition 20
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  0, 0,-1,
				  1, 1, 0,
				  1,-1, 0;
            return 20;
		}
		if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,0.0)) // Condition 21
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Tetragonal;
			P <<  0, 0, 1,
				  1, 0, 0,
				  0, 1, 0;
            return 21;
		}
		if (!typeI && equal(D,-B/2) && equal(E,0.0) && equal(F,0.0)) // Condition 22
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Hexagonal;
			P <<  0, 0, 1,
				  1, 0, 0,
				  0, 1, 0;
            return 22;
		}
		if (!typeI && equal(E,0.0) && equal(F,0.0)) // Condition 23
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Orthorhombic;
			P <<  0, 0, 1,
				  1,-1, 0,
				  1, 1, 0;
            return 23;
		}
		if (!typeI && equal(A+B,2.0*std::fabs(D+E+F)) && equal(E,-A/3) && equal(F,-A/3)) // Condition 24
		{
			centring=LatticeCentring::R;
			bravais=BravaisType::Hexagonal;
			P <<  1, 0, 1,
				  2,-1, 0,
				  1, 1, 0;
            return 24;
		}
		if (!typeI && equal(E,F)) // Condition 25
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  0, 0, 1,
				  1,-1, 0,
				  1, 1, 0;
            return 25;
		}

        throw std::runtime_error("B=C but could not classify cell type");
	}
	else
	{
		if (typeI && equal(D,A/4) && equal(E,A/2) && equal(F,A/2)) // Condition 26
		{
			centring=LatticeCentring::F;
			bravais=BravaisType::Orthorhombic;
			P <<  1,-1,-1,
				  0, 2, 0,
				  0, 0, 2;
            return 26;
		}
		if (typeI && equal(E,A/2) && equal(F,A/2)) // Condition 27
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P << -1,-1, 0,
				  2, 0,-1,
				  0, 0, 1;
            return 27;
		}
		if (typeI && equal(E,A/2) && equal(F,2*D)) // Condition 28
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P << -1,-1, 0,
				  0, 0, 1,
				  0, 2, 0;
            return 28;
		}
		if (typeI && equal(E,2*D) && equal(F,A/2)) // Condition 29
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  1, 1, 0,
				  0,-2, 0,
				  0, 0,-1;
            return 29;
		}
		if (typeI && equal(D,B/2) && equal(F,2*E)) // Condition 30
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  0, 0,-1,
				  1, 1, 0,
				  0,-2, 0;
            return 30;
		}
		if (typeI) // Condition 31
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Triclinic;
			P <<  1, 0, 0,
				  0, 1, 0,
				  0, 0, 1;
            return 31;
		}
		if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,0.0)) // Condition 32
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Orthorhombic;
			P <<  1, 0, 0,
				  0, 1, 0,
				  0, 0, 1;
            return 32;
		}
		if (!typeI && equal(D,-B/2) && equal(E,0.0) && equal(F,0.0)) // Condition 40
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Orthorhombic;
			P <<  0, 0,-1,
				 -1, 1, 0,
				  0, 2, 0;
            return 40;
		}
		if (!typeI && equal(E,0.0) && equal(F,0.0)) // Condition 35
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Monoclinic;
			P <<  0,-1, 0,
				 -1, 0, 0,
				  0, 0,-1;
            return 35;
		}
		if (!typeI && equal(D,0.0) && equal(E,-A/2) && equal(F,0.0)) // Condition 36
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Orthorhombic;
			P <<  1,-1, 0,
				  0, 0, 1,
				  0,-2, 0;
            return 36;
		}
		if (!typeI && equal(D,0.0) && equal(F,0.0)) // Condition 33
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Monoclinic;
			P <<  1, 0, 0,
				  0, 1, 0,
				  0, 0, 1;
            return 33;
		}
		if (!typeI && equal(D,0.0) && equal(E,0.0) && equal(F,-A/2)) // Condition 38
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Orthorhombic;
			P << -1, 1, 0,
				  0, 2, 0,
				  0, 0,-1;
            return 38;
		}
		if (!typeI && equal(D,0.0) && equal(E,0.0)) // Condition 34
		{
			centring=LatticeCentring::P;
			bravais=BravaisType::Monoclinic;
			P << -1, 0, 0,
				  0, 0,-1,
				  0,-1, 0;
            return 34;
		}
		if (!typeI && equal(D,-B/2) && equal(E,-A/2) && equal(F,0.0)) // Condition 42
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Orthorhombic;
			P << -1, 0, 1,
				  0,-1, 1,
				  0, 0, 2;
            return 42;
		}
		if (!typeI && equal(D,-B/2) && equal(F,0.0)) // Condition 41
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  0, 0,-1,
				 -1,-1, 0,
				 -2, 0, 0;
            return 41;
		}
		if (!typeI && equal(E,-A/2) && equal(F,0.0)) // Condition 37
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P <<  1, 1, 0,
				  0, 0, 1,
				  2, 0, 0;
            return 37;
		}
		if (!typeI && equal(E,0.0) && equal(F,-A/2)) // Condition 39
		{
			centring=LatticeCentring::C;
			bravais=BravaisType::Monoclinic;
			P << -1,-1, 0,
				 -2, 0, 0,
				  0, 0,-1;
            return 39;
		}
		if (!typeI && equal(A+B,2.0*std::fabs(D+E+F)) && equal(B,std::fabs(2*D+F))) // Condition 43
		{
			centring=LatticeCentring::I;
			bravais=BravaisType::Monoclinic;
			P << -1,-1, 0,
				  0,-1,-1,
				  0,-2, 0;
            return 43;
		}
		if (!typeI) // Condition 44
		{
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

}
}
