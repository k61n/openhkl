/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
	BP 156
	6, rue Jules Horowitz
	38042 Grenoble Cedex 9
	France
	chapon[at]ill.fr
    pellegrini[at]ill.fr

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_SIMPLEPEAK_H_
#define NSXTOOL_SIMPLEPEAK_H_
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <memory>
#include "IShape.h"
#include "Basis.h"
namespace SX
{
namespace Geometry
{


class IData
{
public:
	IData()
	{}
	// Return the intensity at point x,y,z.
	virtual int dataAt(int x, int y, int z)=0;
	virtual ~IData()=0;
};


class Peak3D
{
public:
	Peak3D(IData* data=0);
	~Peak3D();
	//! Attach the data
	void linkData(IData* data);
	//! Detach the data
	void unlinkData();
	//! Set the Peak region. Peak shaped is owned after setting
	void setPeak(IShape<double,3>* peak);
	//! set the background region. Bkg region is owned after setting
	void setBackground(IShape<double,3>* background);
	//! Set the Miller indices of the peak (double to allow integration of incommensurate peaks)
	void setMillerIndices(double h, double k, double l);
	//! Get the Miller indices of the peak (double to allow integration of incommensurate peaks)
	const Eigen::RowVector3d& getMillerIndices() const;
	//! Set the q vector in the frame of reference of the diffractometer
	void setQ(const Eigen::RowVector3d& q);
	//! Get q vector in the frame of reference of the diffractometer
	const Eigen::RowVector3d& getQ() const;
	//! Run the integration of the peak; iterate over the data
	void integrate();

	void setGammaNu(double gamma,double nu);
	//! Get the projection of total data in the bounding box
	const Eigen::VectorXd& getProjection() const;
	const Eigen::VectorXd& getPeakProjection() const;
	const Eigen::VectorXd& getBkgProjection() const;
	const IShape<double,3>* getPeak() const { return _peak;}
	const IShape<double,3>* getBackground() const {return _bkg;}
	//! Total intensity in the peak
   	double peakTotalCounts() const;
   	//!
   	double getLorentzFactor() const;
//	//! Total intensity in the background
//	double bkgTotalCounts() const;
//	//! Volume of the peak
//	double peakVolume() const;
//	//!  Volume of the background
//	double bkgVolume() const;
	void setBasis(std::shared_ptr<Basis> basis);
private:
	//! Pointer to the data containing the peak
	IData* _data;
	//! Miller indices of the peak
	Eigen::RowVector3d _hkl;
	//! Q Vector
	Eigen::RowVector3d _q;
	//! Shape describing the Peak zone
	IShape<double,3>* _peak;
	//! Shape describing the background zone (must fully contain peak)
	IShape<double,3>* _bkg;
	//!
	Eigen::VectorXd _projection;
	Eigen::VectorXd _projectionPeak;
	Eigen::VectorXd _projectionBkg;
	std::shared_ptr<SX::Geometry::Basis> _basis;
	double _gamma,_nu;
};

}
}
#endif /* NSXTOOL_SIMPLEPEAK_H_ */
