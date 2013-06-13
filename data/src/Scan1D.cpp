#include "Scan1D.h"
#include <stdexcept>

namespace SX
{

Scan1D::Scan1D(const char* xname,const char* yname):_xname(xname),_yname(yname)
{

}
Scan1D::Scan1D(char* xname,char* yname,const vdouble& x, const vdouble& y, const vdouble& e):_xname(xname),_yname(yname)
{
	_x.resize(x.size());
	_y.resize(y.size());
	_e.resize(e.size());
	std::copy(x.begin(),x.end(),_x.begin());
	std::copy(y.begin(),y.end(),_y.begin());
	std::copy(e.begin(),e.end(),_e.begin());

}

Scan1D::~Scan1D()
{

}

void Scan1D::setData(const vdouble& x, const vdouble& y, const vdouble& e)
{
	if (x.size()!=y.size() || x.size()!=e.size())
		throw std::runtime_error("Problem with number of points in scan");
	//
	_x.resize(x.size());
	_y.resize(x.size());
	_e.resize(x.size());
	std::copy(x.begin(),x.end(),_x.begin());
	std::copy(y.begin(),y.end(),_y.begin());
	std::copy(e.begin(),e.end(),_e.begin());
}

const vdouble& Scan1D::getX() const
{
	return _x;
}
const vdouble& Scan1D::getY() const
{
	return _y;
}
const vdouble& Scan1D::getE() const
{
	return _e;
}

vdouble& Scan1D::getX()
{
	return _x;
}
vdouble& Scan1D::getY()
{
	return _y;
}
vdouble& Scan1D::getE()
{
	return _e;
}

std::size_t Scan1D::npoints() const
{
	return _x.size();
}

void Scan1D::clear()
{
	_x.clear();
	_y.clear();
	_e.clear();
}



} /* namespace SX */
