#include "Indexer.h"

namespace SX{
	namespace Crystal{


Indexer::Indexer(ptrUnitCell cell):
		_cell(cell),
		_dtol(0.01),
		_angletol(0.01)
{
	auto hkls = _cell->generateReflectionsInSphere(2.0);

	auto UB = _cell->getReciprocalStandardM().transpose();
	for (const auto& hkl : hkls)
	{
		double qnorm = (UB*hkl).norm();
		_possiblePeaks.insert(std::multimap<double,Eigen::Vector3d>::value_type(qnorm,hkl));
	}
}

Indexer::~Indexer()
{

}

std::vector<Eigen::Vector3d> Indexer::index(const Peak3D& peak)
{

	std::vector<Eigen::Vector3d> hkls;
	// First d* comparison
	auto thisQ=peak.getQ();
	double thisdstar=thisQ.norm();
	auto b=_possiblePeaks.lower_bound(thisdstar*(1.0-_dtol));
	auto e=_possiblePeaks.upper_bound(thisdstar*(1.0+_dtol));
	if (b!=_possiblePeaks.end())
	{
		for (auto it=b;it!=e;++it)
		{
			//First peak just compare d*
			if (!_peaks.size())
			{
				hkls.push_back((*it).second);
			}
			else // Need to compare angles with previously stored peaks
			{
				for (auto old: _peaks)
				{
					auto oldQ=old->getQ();
					double angle=acos(thisQ.dot(oldQ)/thisdstar/oldQ.norm());
					double angle2=_cell->getAngle((*it).second,old->getMillerIndices());
					if (angle>(1.0-_angletol)*angle2 && angle<(1.0+_angletol)*angle2)
					{
						hkls.push_back((*it).second);
					}
				}
			}
		}
	}

	return hkls;

}

void Indexer::storePeak(Peak3D* peak)
{
	_peaks.push_back(peak);
	if (_peaks.size()==2)
	{

	}
}

}
}
