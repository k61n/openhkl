#ifndef NSXTOOL_UVECTOR_H_
#define NSXTOOL_UVECTOR_H_

#include <boost/numeric/ublas/vector.hpp>

namespace nsx
{

namespace Geometry
{

typedef boost::numeric::ublas::bounded_vector<long,2> V2L;
typedef boost::numeric::ublas::bounded_vector<long,3> V3L;
typedef boost::numeric::ublas::bounded_vector<long,4> V4L;

typedef boost::numeric::ublas::bounded_vector<float,2> V2F;
typedef boost::numeric::ublas::bounded_vector<float,3> V3F;
typedef boost::numeric::ublas::bounded_vector<float,4> V4F;

typedef boost::numeric::ublas::bounded_vector<double,2> V2D;
typedef boost::numeric::ublas::bounded_vector<double,3> V3D;
typedef boost::numeric::ublas::bounded_vector<double,3> V4D;

} // end namepace Geometry

} // end namespace nsx

#endif /*NSXTOOL_UVECTOR_H_*/
