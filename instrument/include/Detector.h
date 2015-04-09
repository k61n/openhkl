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

	// Static constructor
	static Detector* create(const std::string& name);

	//! Construct a Detector
	Detector();
	//! Construct a Detector from another Detector
	Detector(const Detector& other);
	//! Construct a Detector with a given name
	Detector(const std::string& name);
	//! Construct a Detector from an XML node
	Detector(const ptree& node);
	//! Return a pointer to a copy of the Detector
	virtual Detector* clone() const=0;
	// Destructor
	virtual ~Detector()=0;

	//! Assignment operator
	virtual Detector& operator=(const Detector& other);

	//! Return the number of columns of the detector.
	virtual unsigned int getNCols() const=0;
	//! Return the number of rows of the detector.
	virtual unsigned int getNRows() const=0;
	//!  Get the minimum row index
	virtual int getMinRow() const=0;
	//!  Get the maximum row index
	virtual int getMaxRow() const=0;
	//!  Get the minimum col index
	virtual int getMinCol() const=0;
	//!  Get the maximum col index
	virtual int getMaxCol() const=0;
	//! Return true whether a given pixel falls inside the detector
	virtual bool hasPixel(double px, double py) const=0;

	//! Return the height of the detector (meters)
	virtual double getHeight() const=0;
	//! Return the width of the detector (meters)
	virtual double getWidth() const=0;

	//! Return the angular height of the detector (radians)
	virtual double getAngularHeight() const=0;
	//! Return the angular width of the detector (radians)
	virtual double getAngularWidth() const=0;

	/**
	 *  @brief Get 2\f$ \theta \f$
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @param si Incident wavenumber
	 */
	double get2Theta(double px, double py, const std::vector<double>& values=std::vector<double>(), const Eigen::Vector3d& si=Eigen::Vector3d(0,1,0)) const;
	//! Get 2\f$ \theta \f$ from an event on the detector
	double get2Theta(const DetectorEvent& event, const Eigen::Vector3d& si) const;
	/**
	 *  @brief Get the position of a scattering event at px, py.
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @return spatial position of this event
	 */
	Eigen::Vector3d getEventPosition(double px, double py,const std::vector<double>& values=std::vector<double>()) const;
	//! Get the position of a scattering event
	Eigen::Vector3d getEventPosition(const DetectorEvent& event) const;
	/**
	 *  @brief Get the scattering angles for an event on the detector
	 *  @param px horizontal position of the scattering event in pixels unit
	 *  @param py vertical position of the scattering event in pixels units
	 *  @param gamma reference to angle in the yx-plane (gamma=0 along y)
	 *  @param nu reference to elevation angle
	 */
	void getGammaNu(double px, double py, double& gamma, double& nu,  const std::vector<double>& values=std::vector<double>(), const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
	//! Get the scattering angles for an event on the detector
	void getGammaNu(const DetectorEvent& event, double& gamma, double& nu,const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;
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
	 * Get the scattered wavenumber for an event on this detector
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
	//! Get the transferred wavenumber for an event on this detector
	Eigen::Vector3d getQ(const DetectorEvent& event, double wave,const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;

	//! Reuturn whether the detector with goniometer values can collect scattering at Kf. If true, px and py would be the pixel coordinates of the event
	bool receiveKf(double& px, double& py,const Eigen::Vector3d& kf,const Eigen::Vector3d& from, const std::vector<double>& values=std::vector<double>());

	//! Returns the number of detector
	virtual unsigned int getNDetectors() const=0;

	//! Create a detector event, a small object with state of the event on the detector and gonio setup
	DetectorEvent createDetectorEvent(double x, double y, const std::vector<double>& goniosetup=std::vector<double>());

	//! Returns the position of a given pixel in detector space. This takes into account the detector motions in detector space.
	virtual Eigen::Vector3d getPos(double x, double y) const=0;

	//! Determine whether detector at rest can receive a scattering event with direction given by Kf. px and py are detector position if true.
	virtual bool hasKf(const Eigen::Vector3d& kf, const Eigen::Vector3d& from, double& px, double& py) const =0;

	//!

};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_DETECTOR_H_ */
