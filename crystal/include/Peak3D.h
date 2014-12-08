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
#include <Eigen/Dense>
#include <map>
#include <memory>
#include "Basis.h"
#include "IShape.h"


namespace SX
{
namespace Data
{
	class IData;
}
namespace Instrument
{
	class Diffractometer;
	class ComponentState;
	class DetectorEvent;
	class Sample;
	class Detector;
}


namespace Crystal
{


class Peak3D
{
public:

	Peak3D(SX::Data::IData* data=0);
	Peak3D(const Peak3D& other);
	Peak3D& operator=(const Peak3D& other);
	~Peak3D();
	//! Attach the data
	void linkData(SX::Data::IData* data);
	//! Detach the data
	void unlinkData();
	//! Set the Peak region. Peak shaped is owned after setting
	void setPeakShape(SX::Geometry::IShape<double,3>* peak);
	//! set the background region. Bkg region is owned after setting
	void setBackgroundShape(SX::Geometry::IShape<double,3>* background);
	//! Set the Miller indices of the peak (double to allow integration of incommensurate peaks)
	void setMillerIndices(double h, double k, double l);
	//! Get the Miller indices of the peak (double to allow integration of incommensurate peaks)
	const Eigen::RowVector3d& getMillerIndices() const;
	//!
	//! Get q vector in the frame of reference of the diffractometer
	Eigen::RowVector3d getQ() const;
	void getGammaNu(double& gamma,double& nu) const;
	//! Run the integration of the peak; iterate over the data
	void integrate();
	//!
	const SX::Data::IData* getData() const { return _data;}
	SX::Data::IData* getData() { return _data;}
	//! Get the projection of total data in the bounding box.
	Eigen::VectorXd getProjection() const;
	Eigen::VectorXd getPeakProjection() const;
	Eigen::VectorXd getBkgProjection() const;
	Eigen::VectorXd getProjectionSigma() const;
	Eigen::VectorXd getPeakProjectionSigma() const;
	Eigen::VectorXd getBkgProjectionSigma() const;
	const SX::Geometry::IShape<double,3>* getPeak() const { return _peak;}
	const SX::Geometry::IShape<double,3>* getBackground() const {return _bkg;}
	//! Return the scaled intensity of the peak.
   	double getScaledIntensity() const;
	//! Return the raw intensity of the peak.
   	double getRawIntensity() const;
	//! Returns the error on the raw intensity.
   	double getRawSigma() const;
	//! Returns the error on the scaled intensity.
   	double getScaledSigma() const;
   	//! Return the lorentz factor of the peak.
   	double getLorentzFactor() const;
    SX::Instrument::ComponentState* getSampleState();
    SX::Instrument::DetectorEvent* getDetectorEvent();
   	//! Return the scaling factor.
   	double getScale() const;
   	//! Rescale the current scaling factor by scale.
   	void rescale(double factor);
   	//! Set the scaling factor.
   	void setScale(double factor);
   	//!
   	void setSampleState(SX::Instrument::ComponentState* gstate);
   	//!
   	void setDetectorEvent(SX::Instrument::DetectorEvent* event);
   	//!
   	void setWavelength(double wave);
   	bool setBasis(std::shared_ptr<SX::Geometry::Basis> basis);
	bool hasIntegerHKL(std::shared_ptr<SX::Geometry::Basis> basis);
	friend bool operator<(const Peak3D& p1, const Peak3D& p2);
	void setSelected(bool);
	bool isSelected() const;
	void setMasked(bool masked);
	bool isMasked() const;
	void setSample(SX::Instrument::Sample*);
	void setDetector(SX::Instrument::Detector*);
private:
	//! Pointer to the data containing the peak
	SX::Data::IData* _data;
	//! Miller indices of the peak
	Eigen::RowVector3d _hkl;
	//! Shape describing the Peak zone
	SX::Geometry::IShape<double,3>* _peak;
	//! Shape describing the background zone (must fully contain peak)
	SX::Geometry::IShape<double,3>* _bkg;
	//!
	Eigen::VectorXd _projection;
	Eigen::VectorXd _projectionPeak;
	Eigen::VectorXd _projectionBkg;
	//!
	std::shared_ptr<SX::Geometry::Basis> _basis;
	//! Pointer to the state of the Sample Component
	SX::Instrument::ComponentState* _sampleState;
	//! Pointer to a Detector Event state
	SX::Instrument::DetectorEvent* _event;
	double _counts, _countsSigma;
	double _wave;
	double _scale;
	bool _selected;
	bool _masked;
};


}
}
#endif /* NSXTOOL_SIMPLEPEAK_H_ */
