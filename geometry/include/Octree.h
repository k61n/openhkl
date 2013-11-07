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

#ifndef NSXTOOL_OCTREE_H_
#define NSXTOOL_OCTREE_H_

#include <vector>

#include "V3D.h"
#include "AABBObject.h"

namespace SX {

namespace Geometry {


class Octree
{
public:

	Octree(const V3D& lBound, const V3D& uBound);

	static void defineParameters(int maxStorage, int maxDepth);

	void addData(AABBObject* data);

	struct Node : public AABBObject
	{
		Node();
		Node(const V3D& lBound, const V3D& uBound);
		~Node();
		void setLimits(const V3D& lBound, const V3D& uBound);
		void setLimits(double xmin,double ymin, double zmin, double xmax, double ymax, double zmax);
		void split(AABBObject* data);
		bool hasChildren() const;
		void addData(AABBObject* data);
		Node* _children;
		std::vector<AABBObject*> _data;
		void printSelf(std::ostream& os) const;

	};
	Octree::Node* getRoot() { return _root;}

	~Octree();

public:
	static int _nSplits;

private:
	static std::size_t _maxStorage;
	static std::size_t _maxDepth;
	Node* _root;
};



} // namespace Geometry

} // namespace SX
#endif /* NSXTOOL_OCTREE_H_ */
