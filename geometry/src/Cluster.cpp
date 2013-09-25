#include "Cluster.h"
#include "Units.h"

using namespace SX::Units;

namespace SX
{
namespace Geometry
{

Cluster::Cluster()
{
    _center = v3D();
    _size = 0.0;
}

// Update the cluster by adding a vector.
// This will recomputes the center on the fly altogether with the size of the cluster.
Cluster::addVector(const V3D & v)
{
	_center = ((_center*_size) + v)/_size++;
}
	
	
/// Constructor [Null]
ClustersCollection::ClustersCollection()
{
	_rMax = 1.0/m;
	_dr = 1.0e-2/m;
}

ClustersCollection::ClustersCollection(const double rMax, const double dr)
{
	_rMax = rMax;
	_dr = dr;
}

void ClustersCollection::addVector(const V3D & v)
{

	double r;
	r = v.norm();
	
	// If the vector has a length > rMax skip it.
	if (r > _rMax)
	{
		return;	
	}
	
	// Compute the bin corresponding to the current vector.
	int bin;
	bin = <int>(r/_dr);
	
	auto match=coll.find(bin);
	if (match==coll.end())
	{
		coll.insert(match, std::pair<int,Cluster>(bin,Cluster()));
	}

	match->second.addVector(v);
				
}
		
} // Namespace Geometry
} // Namespace SX
