#include "NiggliReduction.h"
#include <cmath>

namespace SX
{
namespace Crystal
{

Matrix33<double> NiggliReduction(const Matrix33<double>& g, double epsilon)
{
	if (epsilon<=0)
		throw std::runtime_error("Niggli reduction: epsilon must be >0");
	bool finished=false;
	Matrix33<double> CM(1,0,0,0,1,0,0,0,1);
	// Return matrix
	Matrix33<double> result=g;

	bool first_time=true;
	bool cond1,cond2,cond3;
	do {
		// No need first time since CM is Identity.
		if (!first_time)
		{
		result=result*CM;
		CM.invert();
		result=CM*result;
		}
		first_time=false;
		double A=result(0,0);
		double B=result(1,1);
		double C=result(2,2);
		double zeta=0.5*result(0,1);
		double eta=0.5*result(0,2);
		double xi=0.5*result(1,2);

		int l=0,m=0,n=0;
		// Initialize the angles as acute or obtuse
		if (xi<-epsilon)
			l=-1;
		else if (xi>epsilon)
			l=1;
		if (eta<-epsilon)
			m=-1;
		else if (eta> epsilon)
			m=1;
		if (zeta<-epsilon)
			n=-1;
		else if (zeta>epsilon)
			n=1;
		if (finished)
			break;
		// Now Starts the 8 conditional tests
		// Step 1
		if (A>B+epsilon || (!(std::abs(A-B)>epsilon) && (std::abs(xi)>std::abs(eta)+epsilon)))
		{
			CM.set(0,-1,0,-1,0,0,0,0,-1);
			finished=true;
			continue;
		}
		// Step 2
		if (B>C+epsilon || (!(std::abs(B-C)>epsilon) && (std::abs(eta)>std::abs(zeta)+epsilon)))
		{
			CM.set(-1,0,0,0,0,-1,0,-1,0);
			continue;
		}
		// Step 3
		int lmn=l*m*n;
		if (lmn==1)
		{
			int i=1,j=1,k=1;
			if (l==-1)
				i=-1;
			if (m==-1)
				j=-1;
			if (n==-1)
				k=-1;
			CM.set(i,0,0,0,j,0,0,0,k);
			finished=true;
			continue;
		}
		//Step 4
		if (lmn==0 || lmn==-1)
		{
			int i=1,j=1,k=1;
			if (l==1)
				i=-1;
			if (m==1)
				j=-1;
			if (n==1)
				k=-1;
			int ijk=i*j*k;
			if (ijk==-1)
			{
				if (l==0)
					i=-1;
				if (m==0)
					j=-1;
				if (n==0)
					k=-1;
			}
			CM.set(i,0,0,0,j,0,0,0,k);
			finished=true;
			continue;
		}
		// Step 5
		cond1=std::abs(xi)>B+epsilon;
		cond2=!(std::abs(B-xi)>epsilon) && ((2*eta)<(zeta-epsilon));
		cond3=!(std::abs(B+xi)>epsilon) && (zeta<-epsilon);
		if (cond1 || cond2 || cond3)
		{
			int sign=(xi>0? 1: -1);
			CM.set(1,0,0,0,1,-sign,0,0,1);
			continue;
		}
		// Step 6
		cond1=std::abs(eta)>A+epsilon;
		cond2=!(std::abs(A-eta)>epsilon) && ((2*xi)<(zeta-epsilon));
		cond3=!(std::abs(A+eta)>epsilon) && (zeta<-epsilon);
		if (cond1 || cond2 || cond3)
		{
			int sign=(eta>0? 1: -1);
			CM.set(1,0,-sign,0,1,0,0,0,1);
			continue;
		}
		//Step 7
		cond1=std::abs(zeta)>A+epsilon;
		cond2=!(std::abs(A-zeta)>epsilon) && ((2*xi)<(eta-epsilon));
		cond3=!(std::abs(A+zeta)>epsilon) && (eta<-epsilon);
		if (cond1 || cond2 || cond3)
		{
			int sign=(zeta>0? 1: -1);
			CM.set(1,-sign,0,0,1,0,0,0,1);
			continue;
		}
		//Step 8
		cond1=(xi+eta+zeta+A+B)<-epsilon;
		cond2=!(std::abs(xi+eta+zeta+A+B)>epsilon) && ((2*(A+eta)+zeta)>epsilon);
		if (cond1 || cond2)
		{
			CM.set(1,0,1,0,1,1,0,0,1);
			continue;
		}
	}while(!finished);
	return result;
}

}
}
