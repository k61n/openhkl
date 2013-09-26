#include <vector>
#include "Cluster.h"
#include "Units.h"
#include "V3D.h"

using namespace SX::Units;

namespace SX
{
namespace Geometry
{

Cluster::Cluster()
{
    _center = v3D();
    _size = 0;
    _tolerance = 0.01/am;
}

Cluster::Cluster(double tolerance)
{
	if (tolerance < 0.0)
	{
		throw std::invalid_argument( "received negative value" );
	}

	_center = v3D();
    _size = 0;
    _tolerance = tolerance;
}

Cluster::Cluster(const V3D & v, double tolerance)
{
	if (tolerance < 0.0)
	{
		throw std::invalid_argument( "received negative value" );
	}

	_center = v3D(v);
    _size = 1;
    _tolerance = tolerance;
}


Cluster::addVector(const V3D & vect)
{
	double n2(v.norm2());
	V3D v(_center-_size*vect);
	
	bool b(v.norm2() < -size*_size*_tolerance*_tolerance);
	
	if (b)
	{
		_center += vect;
		_size++;
	}
	
	return b;
}

/////////////////////////////////////////////////
	
UnitCellFinder::UnitCellFinder()
{
	_threshold = 1.0/am;
	_tolerance = 0.1/am;
}	

UnitCellFinder::UnitCellFinder(double threshold, double tolerance)
{
	if (threshold < 0.0 or tolerance < or tolerance > threshold)
	{
		throw std::invalid_argument( "received negative value" );
	}
	_threshold = threshold;
	_tolerance = tolerance;

}

UnitCellFinder::addPeaks(const vector<V3D> p &)
{
	
	if (_peaks.empty())
	{
		_peaks.reserve(p.size());	
	}
	
	for (auto it=p.begin(), it<p.end(), it++)
	{
		_peaks.push_back(*it);	
	}
}

// A voir les cas ou un peak est deja dans la liste
UnitCellFinder::addPeak(double x, double y, double z)
{
    V3D v(x,y,z);
    _peaks.push_back(v);
}

// A faire le removeSinglePeak

void UnitCellFinder::run(void)
{

	V3D diff;
	double norm;	
	
	for (auto it1=peaks.begin(), it1<peaks.end()-1, it1++)
	{
		for (auto it2=it1+1, peaks.end(), it2++)
		{
			diff = *it2 - *it1;
			norm = diff.norm();
			auto itlow = _clusters.lower_bound(norm-_threshold);
			auto itup = _clusters.lower_bound(norm+_threshold);
			
			// No lower bound was found, add a new cluster to the multimap
			if (itlow == _clusters.end())
			{
				_clusters.insert(itlow, std::make_pair(norm,Cluster(diff,_tolerance)));
			}
			else
			{
				for (auto it=itlow, it<itup, it++)
				{
					if (it->second.addVector(diff))
					{
						break;	
					}
					if (it == _clusters.end())
					{
						_clusters.insert(std::make_pair(norm,Cluster(diff,_tolerance)));
					}
				}
			}
		}	
	}	
	
}
		
} // Namespace Geometry
} // Namespace SX
