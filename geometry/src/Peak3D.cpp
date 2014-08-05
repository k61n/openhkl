#include "Peak3D.h"
#include <cmath>

namespace SX
{
namespace Geometry
{

IData::~IData()
{
}

Peak3D::Peak3D(IData* data):
		_data(data),
		_h(0),_k(0),_l(0),
		_peak(nullptr),
		_bkg(nullptr)
{
}

Peak3D::~Peak3D()
{
	//if (_peak)
	//	delete _peak;
	//if (_bkg)
	//	delete _bkg;
}

void Peak3D::linkData(IData* data)
{
	_data=data;
}

void Peak3D::unlinkData()
{
	_data=nullptr;
}

void Peak3D::setPeak(IShape<double,3>* p)
{
	_peak=p;
}

void Peak3D::setBackground(IShape<double,3>* b)
{
	_bkg=b;
}

void Peak3D::setMillerIndices(double h, double k, double l)
{
	_h=h;_k=k;_l=l;
}
void Peak3D::getMillerIndices(double& h, double& k, double& l)
{
	h=_h; k=_k, l=_l;
}

void Peak3D::integrate()
{
	if (!_data)
		return;
	// Get the lower and upper limit of the bkg Bounding box
	const Eigen::Vector3d& lower=_bkg->getLower();
	const Eigen::Vector3d& upper=_bkg->getUpper();

	//
	int data_start=static_cast<int>(std::ceil(lower[2]));
	int data_end=static_cast<int>(std::floor(upper[2]));

	int start_x=static_cast<int>(std::ceil(lower[0]));
	int end_x=static_cast<int>(std::floor(upper[0]));

	int start_y=static_cast<int>(std::ceil(lower[1]));
	int end_y=static_cast<int>(std::floor(upper[1]));

	Eigen::Vector4d point1, point2, point3, point4;

	// Allocate all vectors
	_projection=Eigen::VectorXd::Zero(data_end-data_start+1);
	_projectionPeak=Eigen::VectorXd::Zero(data_end-data_start+1);
	_projectionBkg=Eigen::VectorXd::Zero(data_end-data_start+1);

	for (int z=data_start;z<=data_end;++z)
	{

		for (int x=start_x;x<=end_x;++x)
		{
			for (int y=start_y;y<=end_y;++y)
			{
				int intensity=_data->dataAt(x,y,z);
				_projection[z-data_start]+=intensity;
				point1 << x,y,z,1;
				point2 << x+1,y,z,1;
				point3 << x,y+1,z,1;
				point4 << x+1,y+1,z,1;
				bool inpeak=(_peak->isInside(point1) || _peak->isInside(point2) || _peak->isInside(point3) || _peak->isInside(point4));
				if (inpeak)
					_projectionPeak[z-data_start]+=intensity;
			}
		}
	}
	_projectionBkg=_projection-_projectionPeak;
	return;
}

const Eigen::VectorXd& Peak3D::getProjection() const
{
	return _projection;
}
const Eigen::VectorXd& Peak3D::getPeakProjection() const
{
	return _projectionPeak;
}
const Eigen::VectorXd& Peak3D::getBkgProjection() const
{
	return _projectionBkg;
}

}
}

