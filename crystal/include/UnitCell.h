/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
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

#ifndef NSXTOOL_UNITCELL_H_
#define NSXTOOL_UNITCELL_H_
#include <iostream>
#include <Eigen/Dense>

namespace SX
{

namespace Crystal
{

/** @brief Class to define a crystallographic unit-cell.
 *
 * Provide functionalities to transform vectors of the direct lattice or reciprocal
 * lattice from unit-cell coordinates to a right-handed coordinates system.
 * The UnitCell is defined by the parameters a,b,c and angles alpha, beta, gamma.
 * The UnitCell parameters a,b,c are given in \f$ \AA \f$  and angle alpha, beta, gamma
 * are given in degrees.
 * The a axis is chosen as pointing along the x-direction, and the b-axis is in the xy-plane.
 */
class UnitCell
{
public:
	//! Centering type of the corresponding Bravais lattice
	enum Centring {P,A,B,C,I,F,R};
	/// Constructor
	UnitCell();
	//! Build Unit Cell from lattice parameters. Angles given in radians.
	UnitCell(double a, double b, double c, double alpha, double beta, double gamma, Centring type=P);
	//! Build Unit-cell from 3 non-coplanar vectors. Throw if coplanars.
	UnitCell(const Eigen::Vector3d& v1,const Eigen::Vector3d& v2,const Eigen::Vector3d& v3, Centring type=P);
	//! Copy constructor
	UnitCell(const UnitCell&);
	//! Assignment
	UnitCell& operator=(const UnitCell&);
	//! Destructor
	virtual ~UnitCell();
	/** Set unit-cell parameter
	 * @param a  a-axis (\f$ \AA \f$)
	 * @param b  b-axis (\f$ \AA \f$)
	 * @param c  c-axis (\f$ \AA \f$)
	 * @param alpha Alpha angle in radians
	 * @param beta Beta angle in radians
	 * @param gamma Gamma angle in radians
	 */
	void setCell(double a, double b, double c, double alpha, double beta, double gamma);
	//! Set the a lattice parameter
	void setA(double);
	//! Set the b lattice parameter
	void setB(double);
	//! Set the c lattice parameter
	void setC(double);
	//! Set the \f$ \alpha \f$ lattice parameter
	void setAlpha(double);
	//! Set the \f$ \beta \f$ lattice parameter
	void setBeta(double);
	//! Set the \f$ \gamma \f$ lattice parameter
	void setGamma(double);
	//! Set Bravais lattice type
	void setCentringType(Centring type);
	/** Get the direct-cell volume
	 * @return vol in \f$ \AA^3 \f$
	 */
	double getVolume() const;
	//! Get Lattice Bravais type
	Centring getType() const;
	/// Get the a parameter (\f$ \AA \f$)
	double getA() const;
	/// Get the b parameter (\f$ \AA \f$)
	double getB() const;
	/// Get the c parameter (\f$ \AA \f$)
	double getC() const;
	/// Get the alpha angle (radians)
	double getAlpha() const;
	/// Get the beta angle (radians)
	double getBeta() const;
	/// Get the gamma angle (radians)
	double getGamma() const;
	/** @brief Get the A-matrix
	 *
	 * @return AMatrix
	 * The A-matrix allows to convert from unit-cell fractional coordinates
	 * to coordinates in the associated right-angle axis system. The convention
	 * chosen is a along the x-axis, b in the xy plane, right handed.
	 */
	const Eigen::Matrix3d& getAMatrix() const;
	/** @brief Get the B-matrix
	*
	*  @return BMatrix
	 * The B-matrix allows to convert from reciprocal cell fractional coordinates
	* (h,k,l) to coordinates in the associated right-angle axis system.
	*/
	const Eigen::Matrix3d& getBMatrix() const;
	/** @brief Get the Unitary-transformation Matrix (unitary A-matrix)
	 *
	 * @return TMatrix
	 *  The T-matrix allows to convert
	 */
	const Eigen::Matrix3d& getTMatrix() const;
	//! Return the metric tensor
	const Eigen::Matrix3d& getMetricTensor() const;
	/** @brief Transform a vector in UnitCell coordinates to right handed coordinate system
	 *
	 * @param vect Reference to vector to be transformed
	 */
	void transformA(Eigen::Vector3d& vect) const;
	/** @brief Transform a vector in Reciprocal Lattice coordinates to right handed coordinate system
	 *
	* @param vect Reference to vector to be transformed
	*/
	void transformB(Eigen::Vector3d& vect) const;
	/** @brief Transform a vector in Reciprocal Lattice coordinates to right handed coordinate system
	 *
	* @param vect Reference to vector to be transformed
	*/
	void transformT(Eigen::Vector3d& vect) const;
	//! Return a new UnitCell using the transformation matrix P. P is
	//! written in column from the old basis to the new basis
	UnitCell transformLattice(const Eigen::Matrix3d& P);
	//! Transform the UnitCell using the transformation matrix P. P is
	//! written in column from the old basis to the new basis
	void  transformLatticeInPlace(const Eigen::Matrix3d& P);
	//! Print to a stream
	void printSelf(std::ostream& os) const;
	/** @brief Read Parameters from a stream (verbose)
	 *
	 * @param os Output stream
	 */
	void read(std::istream&);
	//! Apply B-matrix to a vector (i.e. (h,k,l))
	Eigen::Vector3d convertB(const Eigen::Vector3d&);
	//! Apply T-matrix to a vector
	Eigen::Vector3d convertT(const Eigen::Vector3d&);
	//! Apply A-matrix to a vector (i.e. (x,y,z))
	Eigen::Vector3d convertA(const Eigen::Vector3d&);
private:
	void calculatesincos();
	void calculateAMatrix();
	void calculateTMatrix();
	void calculateBMatrix();
	void calculateGTensors();
	void calculateReciprocalParameters();
	void calculateVolume();
	void recalculateAll();
	double _a, _b, _c, _alpha, _beta, _gamma;
	double _astar, _bstar, _cstar, _alphastar, _betastar, _gammastar;
	double _ca,_cb,_cc,_sa,_sb,_sc;
	double _volume;
	Eigen::Matrix3d _A;
	Eigen::Matrix3d _T;
	Eigen::Matrix3d _B;
	//! Metric tensor
	Eigen::Matrix3d _G;
	Centring _type;
};
std::ostream& operator<<(std::ostream&,const UnitCell&);
std::istream& operator>>(std::istream&, UnitCell&);


}

}

#endif
