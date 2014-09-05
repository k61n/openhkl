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

Cluster::Cluster(const Eigen::Vector3d & v, double tolerance):_center(v),_size(1),_tolerance(tolerance)
{
	if (tolerance < 0.0)
	{
		throw std::invalid_argument( "received negative value" );
	}

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

/////////////////////////////////////////////////

LatticeFinder::LatticeFinder(double threshold, double tolerance)
{
	if ( (threshold < 0.0) ||  (tolerance < 0.0) || (tolerance > threshold))
	{
		throw std::invalid_argument( "LatticeFinder:: received invalid argument" );
	}
	_threshold = threshold;
	_tolerance = tolerance;

}

void LatticeFinder::addPoints(const std::vector<Eigen::Vector3d>& p )
{
	
	_peaks.reserve(_peaks.size()+p.size());
	
	for (auto it=p.begin(); it<p.end(); ++it)
	{
		_peaks.push_back(*it);	
	}
}

// A voir les cas ou un peak est deja dans la liste
void LatticeFinder::addPoint(double x, double y, double z)
{
    _peaks.push_back(Eigen::Vector3d(x,y,z));
}

void LatticeFinder::addPoint(const Eigen::Vector3d& v)
{
    _peaks.push_back(v);
}

// A faire le removeSinglePeak

void LatticeFinder::run(double cellmin)
{

	Eigen::Vector3d diff;
	double norm;	
	double rec_max=1.0/cellmin;

	for (auto it1=_peaks.begin(); it1!=_peaks.end(); ++it1)
	{
		for (auto it2=it1+1; it2!=_peaks.end(); ++it2)
		{
			diff = *it2;
			diff-= *it1;
			norm = diff.norm();
			if (norm>rec_max || norm < 0.01)
				continue;
			auto itlow = _clusters.lower_bound(norm-_threshold);
			auto itup = _clusters.upper_bound(norm+_threshold);
			
			// No lower bound was found, add a new cluster to the multimap
			if (itlow == _clusters.end())
			{
				_clusters.insert(std::make_pair(norm,Cluster(diff,_tolerance)));
			}
			else
			{
				auto it=itlow;
				for (; it!=itup; ++it)
				{
					if (it->second.addVector(diff) || it->second.addVector(diff*-1.0))
					{
						break;
					}
				}
				if (it == itup)
				{
					_clusters.insert(std::make_pair(norm,Cluster(diff,_tolerance)));
				}
			}
		}	
	}

	std::multimap<double,Cluster> m;
	for (auto it=_clusters.begin();it!=_clusters.end();++it)
	{
		Eigen::Vector3d tmp=it->second.getCenter();
		double norm=tmp.norm();
		m.insert(std::make_pair(norm,it->second));
	}
	_clusters.clear();
	_clusters.insert(m.begin(),m.end());

}

double LatticeFinder::costFunction(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const Eigen::Vector3d& v3, double epsilon, double delta) const
{

    Eigen::Vector3d v1s, v2s, v3s;
	double q;
	double vol;
	std::vector<int> h(3);
	Eigen::Vector3d zeta, delta1, delta2;
    double temp1, temp2;

    vol = v1.dot(v2.cross(v3));

    v1s = v2.cross(v3);
    v1s /= vol;

    v2s = v3.cross(v1);
    v2s /= vol;

    v3s = v1.cross(v2);
    v3s /= vol;


	double bigQ=0.0;
	int numberOfPeaks=0;
    for (auto it=_clusters.begin(); it != _clusters.end(); ++it)
    {
    	q = 0.0;
    	Eigen::Vector3d center=it->second.getCenter();
		zeta[0] = v1s.dot(center);
		h[0] = round(zeta[0]);

		zeta[1] = v2s.dot(center);
		h[1] = round(zeta[1]);

		zeta[2] = v3s.dot(center);
		h[2] = round(zeta[2]);

    	delta1[0] = std::abs(zeta[0]-h[0]);
    	delta1[1] = std::abs(zeta[1]-h[1]);
    	delta1[2] = std::abs(zeta[2]-h[2]);
    	delta1 -= Eigen::Vector3d::Constant(epsilon);

    	delta2[0] = std::abs(h[0]);
    	delta2[1] = std::abs(h[1]);
    	delta2[2] = std::abs(h[2]);
    	delta2 -= Eigen::Vector3d::Constant(delta);

    	for (int i=0; i<3; i++)
    	{
    		temp1 = std::max(delta1[i],0.0)/epsilon;
    		temp2 = std::max(delta2[i],0.0);

    		q += temp1*temp1 + temp2*temp2;

    	}
    	bigQ+=exp(-2.0*q)*it->second.getSize();
    	numberOfPeaks+=it->second.getSize();
    }

    return bigQ/numberOfPeaks;
}


bool LatticeFinder::determineLattice(Eigen::Vector3d& a, Eigen::Vector3d& b, Eigen::Vector3d& c,int clustermax) const
{
	bool success=false;

	std::vector<Cluster> clust;
	clust.reserve(_clusters.size());
	for (auto it=_clusters.begin();it!=_clusters.end();++it)
		clust.push_back((it->second));

	double scoremax=0.0;
	for (auto it=clust.begin();it!=clust.begin()+clustermax;++it)
	{
		Eigen::Vector3d v1=(*it).getCenter();
		for (auto it2=it+1;it2!=clust.begin()+clustermax;++it2)
		{
			Eigen::Vector3d v2=(*it2).getCenter();
			Eigen::Vector3d t=v1.cross(v2);
			if (t.norm()<1e-2) // Two vectors are collinear
				continue;
			for (auto it3=it2+1;it3!=clust.begin()+clustermax;++it3)
			{
				Eigen::Vector3d v3=(*it3).getCenter();
				double vol=t.dot(v3);
				if (std::abs(vol)<1e-5) // all coplanar
					continue;
				if (vol<0)
					v3*=-1.0;

				double score=costFunction(v1,v2,v3,0.05,5);
				if (score>scoremax)
				{
					a=v1;
					b=v2;
					c=v3;
					success=true;
					scoremax=score;
				}
			}
		}

	}
	return success;
}
		
} // Namespace Geometry
} // Namespace SX
