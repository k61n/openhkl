#ifndef NSXTOOL_UMATRIX_H_
#define NSXTOOL_UMATRIX_H_

#include <boost/numeric/ublas/matrix.hpp>

namespace SX
{

namespace Geometry
{

typedef boost::numeric::ublas::bounded_matrix<long,2,2> M22L;
typedef boost::numeric::ublas::bounded_matrix<long,3,3> M33L;
typedef boost::numeric::ublas::bounded_matrix<long,4,4> M44L;

typedef boost::numeric::ublas::bounded_matrix<float,2,2> M22F;
typedef boost::numeric::ublas::bounded_matrix<float,3,3> M33F;
typedef boost::numeric::ublas::bounded_matrix<float,4,4> M44F;

typedef boost::numeric::ublas::bounded_matrix<double,2,2> M22D;
typedef boost::numeric::ublas::bounded_matrix<double,3,3> M33D;
typedef boost::numeric::ublas::bounded_matrix<double,3,3> M44D;

} // end namepace Geometry

} // end namespace SX

#endif /*NSXTOOL_UMATRIX_H_*/
