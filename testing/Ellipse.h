#ifndef NSXTOOL_ELLIPSE_H_
#define NSXTOOL_ELLIPSE_H_

#include "V3D.h"

namespace SX
{

namespace Geometry
{

class Ellipse
{

public:

	Ellipse();
	Ellipse(const V3D& center, const V3D& semi_axes, const V3D& axis1, const V3D& axis2);
	Ellipse(const Ellipse&);
	Ellipse& operator=(const Ellipse&);
	~Ellipse();

	const V3D& getCenter(void) const;
	const V3D& getSemiAxes(void) const;
	const V3D& getAxis1(void) const;
	const V3D& getAxis2(void) const;

private:

	V3D _center;
	V3D _semi_axes;
	V3D _axis1;
	V3D _axis2;

};

}

}

#endif
