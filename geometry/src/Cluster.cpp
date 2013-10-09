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

Cluster::Cluster(const V3D & v, double tolerance):_center(v),_size(1),_tolerance(tolerance)
{
	if (tolerance < 0.0)
	{
		throw std::invalid_argument( "received negative value" );
	}

}

bool Cluster::operator==(const Cluster& c) const
{
	V3D temp=getCenter();
	temp-=c.getCenter();
	if (temp.norm2()<_tolerance*c._tolerance)
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


bool Cluster::addVector(const V3D & vect)
{
	V3D v(_center-vect*_size);
	double tp=_size*_tolerance;
	bool b=(v.norm2() < tp*tp);
	if (!b)
		return false;
	_center += vect;
	_size++;
		return true;
}

/////////////////////////////////////////////////

UnitCellFinder::UnitCellFinder(double threshold, double tolerance)
{
	if ( (threshold < 0.0) ||  (tolerance < 0.0) || (tolerance > threshold))
	{
		throw std::invalid_argument( "UnitCellFinder:: received invalid argument" );
	}
	_threshold = threshold;
	_tolerance = tolerance;

}

void UnitCellFinder::addPeaks(const std::vector<V3D>& p )
{
	
	_peaks.reserve(_peaks.size()+p.size());
	
	for (auto it=p.begin(); it<p.end(); ++it)
	{
		_peaks.push_back(*it);	
	}
}

// A voir les cas ou un peak est deja dans la liste
void UnitCellFinder::addPeak(double x, double y, double z)
{
    _peaks.push_back(V3D(x,y,z));
}

void UnitCellFinder::addPeak(const V3D& v)
{
    _peaks.push_back(v);
}

// A faire le removeSinglePeak

void UnitCellFinder::run(double cellmin)
{

	V3D diff;
	double norm;	
	double rec_max=1.0/cellmin;

	for (auto it1=_peaks.begin(); it1!=_peaks.end(); ++it1)
	{
		for (auto it2=it1+1; it2!=_peaks.end(); ++it2)
		{
			diff = *it2;
			diff-= *it1;
			norm = diff.norm();
			if (norm>rec_max || norm < 0.03)
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
		V3D tmp=it->second.getCenter();
		double norm=tmp.norm();
		m.insert(std::make_pair(norm,it->second));
	}
	_clusters.clear();
	_clusters.insert(m.begin(),m.end());

}

double UnitCellFinder::costFunction(const V3D& v1, const V3D& v2, const V3D& v3, double epsilon, double delta) const
{

    V3D v1s, v2s, v3s;
	double q;
	double vol;
	std::vector<int> h(3);
	V3D zeta, delta1, delta2;
    double temp1, temp2;

    vol = v1.scalar_prod(v2.cross_prod(v3));

    v1s = v2.cross_prod(v3);
    v1s /= vol;

    v2s = v3.cross_prod(v1);
    v2s /= vol;

    v3s = v1.cross_prod(v2);
    v3s /= vol;


	double bigQ=0.0;
	int numberOfPeaks=0;
    for (auto it=_clusters.begin(); it != _clusters.end(); ++it)
    {
    	q = 0.0;
    	V3D center=it->second.getCenter();
		zeta[0] = v1s.scalar_prod(center);
		h[0] = round(zeta[0]);

		zeta[1] = v2s.scalar_prod(center);
		h[1] = round(zeta[1]);

		zeta[2] = v3s.scalar_prod(center);
		h[2] = round(zeta[2]);

    	delta1[0] = std::abs(zeta[0]-h[0]);
    	delta1[1] = std::abs(zeta[1]-h[1]);
    	delta1[2] = std::abs(zeta[2]-h[2]);
    	delta1 -= epsilon;

    	delta2[0] = std::abs(h[0]);
    	delta2[1] = std::abs(h[1]);
    	delta2[2] = std::abs(h[2]);
    	delta2 -= delta;

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


void UnitCellFinder::determineLattice(int clustermax) const
{


	std::vector<Cluster> clust;
	clust.reserve(_clusters.size());
	for (auto it=_clusters.begin();it!=_clusters.end();++it)
		clust.push_back((it->second));

	for (auto it=clust.begin();it!=clust.begin()+clustermax;++it)
	{
		int i=0;
		V3D v1=(*it).getCenter();
		for (auto it2=it+1;it2!=clust.begin()+clustermax;++it2)
		{
			V3D v2=(*it2).getCenter();
			V3D t=v1.cross_prod(v2);
			if (t.norm()<1e-2) // Two vectors are collinear
				continue;
			for (auto it3=it2+1;it3!=clust.begin()+clustermax;++it3)
			{
				V3D v3=(*it3).getCenter();
				double vol=t.scalar_prod(v3);
				if (std::abs(vol)<1e-5) // all coplanar
					continue;


				Matrix33<double> g_1;
				double g00=v1.scalar_prod(v1);
				double g11=v2.scalar_prod(v2);
				double g22=v3.scalar_prod(v3);
				double g01=v1.scalar_prod(v2);
				double g02=v1.scalar_prod(v3);
				double g12=v2.scalar_prod(v3);
				g_1.set(g00,g01,g02,g01,g11,g12,g02,g12,g22);
				g_1.invert();
				SX::Crystal::NiggliReduction n(g_1,1e-5);
				Matrix33<double> t=n.reduce();

				double a=sqrt(t(0,0));
				double b=sqrt(t(1,1));
				double c=sqrt(t(2,2));
				double gamma=acos(t(0,1)/a/b)/SX::Units::deg;
				double beta=acos(t(0,2)/a/c)/SX::Units::deg;
				double alpha=acos(t(1,2)/b/c)/SX::Units::deg;
				double volr=sqrt(t.determinant());

				double score=costFunction(v1,v2,v3,0.05,5);
				if (score>0.90)
				{
					std::cout << a << " " << b << " " << c << " " << alpha << " " << beta << " " << gamma << "Vol: " << volr << " " << score <<   std::endl;
				}

			}
		}

	}
}
		
} // Namespace Geometry
} // Namespace SX
