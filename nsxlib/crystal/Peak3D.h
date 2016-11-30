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
    j.fisher[at]fz-juelide.de

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

#include <map>
#include <memory>

#include <Eigen/Dense>

#include "UnitCell.h"
#include "IShape.h"
#include "Ellipsoid.h"

namespace SX
{

namespace Geometry
{
class Blob3D;
using sptrShape3D=std::shared_ptr<SX::Geometry::IShape<double,3>>;
using Ellipsoid3D=Ellipsoid<double,3>;
using sptrEllipsoid3D=std::shared_ptr<Ellipsoid3D>;
}

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
	class Source;
}

using namespace SX::Geometry;

namespace Crystal
{

class Peak3D
{
public:

    Peak3D(std::shared_ptr<SX::Data::IData> data=std::shared_ptr<SX::Data::IData>());
    Peak3D(std::shared_ptr<SX::Data::IData> data, const SX::Geometry::Blob3D& blob, double confidence);

    //! Copy constructor
    Peak3D(const Peak3D& other);

	//! Assignment operator
	Peak3D& operator=(const Peak3D& other);

	//! Destructor
	~Peak3D();

	//! Attach the data
    void linkData(std::shared_ptr<SX::Data::IData> data);

	//! Detach the data
	void unlinkData();

	//! Set the Peak region.
	void setPeakShape(sptrShape3D peak);

	//! set the background region.
	void setBackgroundShape(sptrShape3D background);

	//! Set the Miller indices of the peak (double to allow integration of incommensurate peaks)
	void setMillerIndices(double h, double k, double l);

	//! Get the Miller indices of the peak (double to allow integration of incommensurate peaks)
	const Eigen::RowVector3d& getMillerIndices() const;

    //! Get the integral Miller indices
    Eigen::RowVector3i getIntegerMillerIndices() const;

	//! Get kf vector in the frame of reference of the diffractometer
	Eigen::RowVector3d getKf() const;

	//! Get q vector in the frame of reference of the diffractometer
	Eigen::RowVector3d getQ() const;

	void getGammaNu(double& gamma,double& nu) const;

	//! Run the integration of the peak; iterate over the data
	void integrate();

	//!
    const std::shared_ptr<SX::Data::IData> getData() const { return _data;}

    std::shared_ptr<SX::Data::IData> getData() { return _data;}

	//! Get the projection of total data in the bounding box.
	Eigen::VectorXd getProjection() const;
	Eigen::VectorXd getPeakProjection() const;
	Eigen::VectorXd getBkgProjection() const;
	Eigen::VectorXd getProjectionSigma() const;
	Eigen::VectorXd getPeakProjectionSigma() const;
	Eigen::VectorXd getBkgProjectionSigma() const;

	std::shared_ptr<SX::Geometry::IShape<double,3>> getPeak();
	std::shared_ptr<SX::Geometry::IShape<double,3>> getBackground();

	//! Return the scaled intensity of the peak.
   	double getScaledIntensity() const;
	//! Return the raw intensity of the peak.
   	double getRawIntensity() const;
	//! Returns the error on the raw intensity.
   	double getRawSigma() const;
	//! Returns the error on the scaled intensity.
   	double getScaledSigma() const;
   	//!
   	double getIOverSigmaI() const;
   	//! Return the lorentz factor of the peak.
   	double getLorentzFactor() const;
    std::shared_ptr<SX::Instrument::ComponentState> getSampleState();
    double getSampleStepSize() const;
    std::shared_ptr<SX::Instrument::DetectorEvent> getDetectorEvent();
   	//! Return the scaling factor.
   	double getScale() const;
   	//! Rescale the current scaling factor by scale.
   	void rescale(double factor);
   	//! Set the scaling factor.
   	void setScale(double factor);
   	//!
    void setSampleState(std::shared_ptr<SX::Instrument::ComponentState> gstate);
   	//!
    void setDetectorEvent(std::shared_ptr<SX::Instrument::DetectorEvent> event);
   	//!
    void setSource(std::shared_ptr<SX::Instrument::Source> source);
   	bool setUnitCell(std::shared_ptr<SX::Crystal::UnitCell> basis);
   	std::shared_ptr<SX::Crystal::UnitCell> getUnitCell() const;
	bool hasIntegerHKL(const SX::Crystal::UnitCell& basis, double tolerance=0.2);
	friend bool operator<(const Peak3D& p1, const Peak3D& p2);
	void setSelected(bool);
	bool isSelected() const;
	void setMasked(bool masked);
	bool isMasked() const;
	void setObserved(bool observed);
	bool isObserved() const;
	void setTransmission(double transmission);
	double getTransmission() const;

    void scalePeakShape(double scale);
    void scaleBackgroundShape(double scale);

    // testing: new implementation of integration
    void framewiseIntegrateBegin();
    void framewiseIntegrateStep(Eigen::MatrixXi& frame, unsigned int idx);
    void framewiseIntegrateEnd();

    //! compute P value that there is actually an observed peak, assuming Poisson statistics
    double pValue();

private:
	//! Pointer to the data containing the peak
    std::shared_ptr<SX::Data::IData> _data;
	//! Miller indices of the peak
	Eigen::RowVector3d _hkl;
	//! Shape describing the Peak zone
	std::shared_ptr<SX::Geometry::IShape<double,3>> _peak;
	//! Shape describing the background zone (must fully contain peak)
	std::shared_ptr<SX::Geometry::IShape<double,3>> _bkg;
	//!
	Eigen::VectorXd _projection;
	Eigen::VectorXd _projectionPeak;
	Eigen::VectorXd _projectionBkg;
    Eigen::VectorXd _pointsPeak;
    Eigen::VectorXd _pointsBkg;
    Eigen::VectorXd _countsPeak;
    Eigen::VectorXd _countsBkg;
	//!
	std::shared_ptr<SX::Crystal::UnitCell> _basis;
	//! Pointer to the state of the Sample Component
    std::shared_ptr<SX::Instrument::ComponentState> _sampleState;
	//! Pointer to a Detector Event state
    std::shared_ptr<SX::Instrument::DetectorEvent> _event;
	//!
    std::shared_ptr<SX::Instrument::Source> _source;
	double _counts;
	double _countsSigma;
	double _scale;
	bool _selected;
	bool _masked;
	bool _observed;
	double _transmission;

    
    struct IntegrationState
    {
        Eigen::Vector3d lower;
        Eigen::Vector3d upper;

        unsigned int data_start;
        unsigned int data_end;

        unsigned int start_x;
        unsigned int end_x;

        unsigned int start_y;
        unsigned int end_y;

        Eigen::Vector4d point1;

        int dx;
        int dy;
    };

    IntegrationState _state;

};

using sptrPeak3D = std::shared_ptr<Peak3D>;

} // namespace Crystal

} // namespace SX

#endif /* NSXTOOL_SIMPLEPEAK_H_ */
