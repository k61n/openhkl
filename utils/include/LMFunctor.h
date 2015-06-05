/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
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

#ifndef NSXTOOL_LMFUNCTOR_H_
#define NSXTOOL_LMFUNCTOR_H_

namespace SX
{

namespace Utils
{

/** @brief Functor that complies with the interface of Eigen Minimizer
 *
 * Nx : number of inputs (parameters) at compile time
 * Ny : number of values (points) at compile time
 */
template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
class LMFunctor
{
public:
	typedef _Scalar Scalar;
	enum {InputsAtCompileTime = NX,ValuesAtCompileTime = NY};
	typedef Eigen::Matrix<Scalar,InputsAtCompileTime,1> InputType;
	typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,1> ValueType;
	typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,InputsAtCompileTime> JacobianType;

	LMFunctor();
	LMFunctor(int inputs, int values);

	virtual int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const=0;

	virtual int inputs() const;
	virtual int values() const;
	virtual ~LMFunctor()=0;;

protected:
	int m_inputs, m_values;

};

template<typename _Scalar, int NX, int NY>
LMFunctor<_Scalar,NX,NY>::LMFunctor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime)
{
}

template<typename _Scalar, int NX, int NY>
LMFunctor<_Scalar,NX,NY>::LMFunctor(int inputs, int values) : m_inputs(inputs), m_values(values)
{
}

template<typename _Scalar, int NX, int NY>
LMFunctor<_Scalar,NX,NY>::~LMFunctor()
{
}

template<typename _Scalar, int NX, int NY>
int LMFunctor<_Scalar,NX,NY>::inputs() const
{
	return m_inputs;
}

template<typename _Scalar, int NX, int NY>
int LMFunctor<_Scalar,NX,NY>::values() const {
	return m_values;
}

} // end namespace Utils

} // end namespace SX

#endif /* NSXTOOL_LMFUNCTOR_H_ */
