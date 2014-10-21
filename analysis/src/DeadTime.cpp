#include <math.h>
#include <cmath>
#include <stdexcept>
#include <algorithm>
// Eric: add this to make bind2nd and divides work on win32
#include <functional>

#include "DeadTime.h"

namespace SX
{

DTCorrection::DTCorrection(const char* name):_name(name)
{

}

DTCorrection::~DTCorrection()
{

}
void DTCorrection::setTerm(unsigned int N, double alpha, double salpha)
{
	if (N>_ai.size())
	{
		_ai.resize(N);
	}
	_ai[N-1]=std::pair<double,double>(alpha,salpha);
}

std::pair<double,double> DTCorrection::getTerm(unsigned int N) const
{
	if (N>_ai.size())
		throw std::range_error("Deadtime  is of smaller rank");
	return _ai[N-1];
}

void DTCorrection::apply(double& ccount,double& sccount, double time) const
{
	// Do nothing, no corrective terms
	if (_ai.empty())
		return;
	//
	if (std::fabs(time)<1e-9)
		throw std::runtime_error("Time too short");
	double count=ccount;
	double scount=sccount;
	double ratio=count/time;
	// Calculate denominator
	double denom=1.0;
	double dydn=0;
	// Create a vector that will contains partial derivatives dy/dai where ai are expansion terms
	std::vector<double> dydai(_ai.size());
	std::fill(dydai.begin(),dydai.end(),0.0);
	for (unsigned int i=0;i<_ai.size();++i)
	{
		double power=std::pow(ratio,i+1);
		double expans=_ai[i].first*power;
		denom-=expans;
		dydn+=expans*(i+1);
		dydai[i]=count*power;
	}
	if (denom<0)
		throw std::runtime_error("Problem with deadtime correction coeficients. Leads to negative correction");
	dydn+=denom;
	double denom2=denom*denom;
	dydn/=denom2;
	std::transform(dydai.begin(),dydai.end(),dydai.begin(),std::bind2nd(std::divides<double>(),denom2));
	// Now calculate
	ccount=count/denom;
	sccount=dydn*dydn*scount*scount;
	for (unsigned int i=0;i<_ai.size();++i)
	{
		sccount+=dydai[i]*_ai[i].second*_ai[i].second;
	}
	sccount=sqrt(sccount);

}



} // End namespace SX


