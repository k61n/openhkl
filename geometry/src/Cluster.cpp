#include <algorithm>
#include <cmath>
#include <vector>
#include <stdexcept>
#include "Cluster.h"
#include "Matrix33.h"
#include "Units.h"
#include "NiggliReduction.h"

namespace SX
{
namespace Geometry
{

Cluster::Cluster():_center(),_size(0),_tolerance(0.01)
{

}

Cluster::Cluster(double tolerance):_center(),_size(0),_tolerance(tolerance)
{
	if (tolerance < 0.0)
	{
		throw std::invalid_argument( "received negative value" );
	}
}
Cluster::Cluster(const Cluster& rhs)
{
	_center=rhs._center;
	_size=rhs._size;
	_tolerance=rhs._tolerance;
}
Cluster& Cluster::operator=(const Cluster& rhs)
{
	if (this!=&rhs)
	{
		_center=rhs._center;
		_size=rhs._size;
		_tolerance=rhs._tolerance;
	}
	return *this;
}

Cluster::Cluster(const Eigen::Vector3d & v, double tolerance):_center(v),_size(1),_tolerance(tolerance)
{
	if (tolerance < 0.0)
	{
		throw std::invalid_argument( "received negative value" );
	}

}

Eigen::Vector3d Cluster::getCenter() const
{
	return _center/static_cast<double>(_size);
}

bool Cluster::operator==(const Cluster& c) const
{
	Eigen::Vector3d temp=getCenter();
	temp-=c.getCenter();
	if (temp.squaredNorm()<_tolerance*c._tolerance)
		return true;
	return false;
}

Cluster& Cluster::operator+=(const Cluster& c)
{
	_center+=c._center;
	_size+=c._size;
	_tolerance+=c._tolerance;
	_tolerance/=2.0;
	return *this;
}


bool Cluster::addVector(const Eigen::Vector3d& vect)
{
	Eigen::Vector3d v(_center-vect*_size);
	double tp=_size*_tolerance;
	bool b=(v.squaredNorm() < tp*tp);
	if (!b)
		return false;
	_center += vect;
	_size++;
	return true;
}

		
} // Namespace Geometry
} // Namespace SX
