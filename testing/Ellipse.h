#ifndef NSXTOOL_ELLIPSE_H_
#define NSXTOOL_ELLIPSE_H_
#include <Eigen/Dense>

namespace SX
{

namespace Geometry
{
using Eigen::Vector3d;

class Ellipse
{

public:

	Ellipse();
	Ellipse(const Vector3d& center, const Vector3d& semi_axes, const Vector3d& axis1, const Vector3d& axis2);
	Ellipse(const Ellipse&);
	Ellipse& operator=(const Ellipse&);
	~Ellipse();

	const Vector3d& getCenter(void) const;
	const Vector3d& getSemiAxes(void) const;
	const Vector3d& getAxis1(void) const;
	const Vector3d& getAxis2(void) const;

private:

	Vector3d _center;
	Vector3d _semi_axes;
	Vector3d _axis1;
	Vector3d _axis2;

};

}

}

#endif
