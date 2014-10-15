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

#ifndef NSXTOOL_DETECTOR_H_
#define NSXTOOL_DETECTOR_H_

#include <string>
#include <vector>

#include <Eigen/Dense>

#include "Component.h"
#include "DetectorEvent.h"

namespace SX
{

namespace Instrument
{

typedef unsigned int uint;

/** @brief Base class for Detectors.
 *
 *
 */
class Detector : public Component
{
public:
	//! Default constructor
	Detector();
	//! Copy constructor
	Detector(const Detector& other);
	Detector(const std::string& name);
	// Destructor
	virtual ~Detector()=0;
	//! Assignment operator
	virtual Detector& operator=(const Detector& other);
	//! Set the dimensions of the detector (meters).
	void setDimensions(double width, double height);
	//! Set sample to detector distance (overwrites Component::setRestPosition)
	void setDistance(double d);
	//! Set the rest position of the detector (along y in Busing Levy convention)
	void setRestPosition(const Eigen::Vector3d& p);
	//! Set the width (meters)
	void setWidth(double width);
	//! Set the height (meters)
	void setHeight(double height);
	//! Returns the detector height.
	double getHeigth() const;
	//! Returns the detector width
	double getWidth() const;
	//! Set the size of the detector using angular units (radians) rather than lengths. Converted internally in width and height.
	//! Use Units::deg for easy conversion
	virtual void setAngularRange(double widthAngle, double heightAngle)=0;
	//! Set the full width of the detector in Angle, assume the distance is set before
	virtual void setWidthAngle(double wangle)=0;
	//! Set the full height using angular dimension
	virtual void setHeightAngle(double hangle)=0;
	//! Return the width in angular units (radians) covered by the detector
	virtual double getWidthAngle() const=0;
	//! Return the height in angular units (radians) covered by the detector
	virtual double getHeightAngle() const=0;
	//! Set the number of pixels of the detector
	void setNPixels(unsigned int cols,unsigned int rows);
	//! Set the number of columns
	void setNCols(unsigned int cols);
	//! Set the number of rows
	void setNRows(unsigned int rows);
	//! Returns the number of columns of the detector.
	int getNCols() const;
	//! Returns the number of rows of the detector.
	int getNRows() const;
	//! Returns the height of a detector pixel.
	double getPixelHeigth() const;
	//! Returns the width of a detector pixel.
	double getPixelWidth() const;
	/**
	 *  @brief Get the position of a scattering event at px, py.
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @return spatial position of this event
	 */
	Eigen::Vector3d getEventPosition(double px, double py,const std::vector<double>& values=std::vector<double>()) const;
	Eigen::Vector3d getEventPosition(const DetectorEvent& event) const;
	/**
	 *  @brief Get the scattered wavenumber for an event on a detector
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @param wave incident wavelength in \f$ \AA^{-1} \f$
	 *  @param from Optional scattering point position
	 *  @return Scattered wavenumber s=\f$ \frac{k_f}{2\pi} \f$
	 */
	Eigen::Vector3d getKf(double px, double py,double wave, const std::vector<double>& values=std::vector<double>(), const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
	/**
	 *
	 */
	Eigen::Vector3d getKf(const DetectorEvent& event,double wave, const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
	/**
	 *  @brief Get the transferred wavenumber for an event on a detector
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @param si incident wavenumber si=\f$ \frac{k_i}{2\pi} \f$
	 *  @param from Optional scattering point position
	 *  @return Transferred wavenumber s=\f$ \frac{k_f-k_i}{2\pi} \f$
	 */
	Eigen::Vector3d getQ(double px, double py,double wave, const std::vector<double>& values=std::vector<double>(), const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
	Eigen::Vector3d getQ(const DetectorEvent& event, double wave,const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
	/**
	 *  @brief Get the scattering angles for an event on the detector
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @param gamma reference to angle in the yx-plane (gamma=0 along y)
	 *  @param nu reference to elevation angle
	 */
	void getGammaNu(double px, double py, double& gamma, double& nu,  const std::vector<double>& values=std::vector<double>(), const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
	void getGammaNu(const DetectorEvent& event, double& gamma, double& nu,const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
	/**
	 *  @brief Get 2\f$ \theta \f$
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @param si Incident wavenumber
	 */
	double get2Theta(double px, double py, const std::vector<double>& values=std::vector<double>(), const Eigen::Vector3d& si=Eigen::Vector3d(0,1,0)) const;
	double get2Theta(const DetectorEvent& event, const Eigen::Vector3d& si) const;
	//! Pointer to function that maps data indexing with detector indexing
	void setDataMapping(std::function<void(double,double,double&,double&)>);
	//! Create a detector event, a small object with state of the event on the detector and gonio setup
	DetectorEvent createDetectorEvent(double x, double y, const std::vector<double>& goniosetup=std::vector<double>());
	virtual void parse(const ptree&)=0;
	//
protected:
	void convertCoordinates(double, double , double&, double&) const;
	uint _nRows, _nCols;
	double _width, _height;
	// Sample to detector distance
	double _distance;
	std::function<void(double,double,double&,double&)> _mapping;
private:
	virtual Eigen::Vector3d getPos(double x, double y) const=0;
};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_DETECTOR_H_ */
