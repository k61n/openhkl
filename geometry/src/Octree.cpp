#include "Octree.h"

namespace SX {
namespace Geometry {


int Octree::_nSplits(0);
std::size_t Octree::_maxStorage(5);
std::size_t Octree::_maxDepth(10);

Octree::Octree(const V3D& lBound, const V3D& uBound)
{
	_root = new Node(lBound, uBound);
}

void Octree::defineParameters(int maxStorage, int maxDepth)
{
	_maxStorage = maxStorage;
	_maxDepth = maxDepth;

}

void Octree::addData(AABBObject* data)
{
	_root->addData(data);
}

void Octree::Node::addData(AABBObject* data)
{

	if (!intercept(*data))
		return;

	if (hasChildren())
	{
		for (int i=0; i<8; ++i)
		{
			_children[i].addData(data);
		}
	}
	else
	{
		if (_data.size() < _maxStorage)
		{
			_data.push_back(data);
		} else
		{
			split(data);
		}
	}

}

Octree::Node::Node() : AABBObject(), _children(nullptr)
{
	_data.reserve(_maxStorage);
}

Octree::Node::Node(const V3D& lBound, const V3D& uBound) : AABBObject(lBound,uBound), _children(nullptr)
{
	_data.reserve(_maxStorage);
}

void Octree::Node::setLimits(const V3D& lBound, const V3D& uBound)
{
	_lowerBound(lBound);
	_upperBound(uBound);
}

void Octree::Node::setLimits(double xmin,double ymin, double zmin, double xmax, double ymax, double zmax)
{
	_lowerBound(xmin,ymin,zmin);
	_upperBound(xmax,ymax,zmax);

}

bool Octree::Node::hasChildren() const
{
	return (_children != nullptr);
}

void Octree::Node::split(AABBObject* data)
{

	_nSplits++;

	_children = new Node[8];

	V3D v2((_upperBound+_lowerBound)*0.5);

	bool x,y,z;
	for (int i=0; i<8; ++i)
	{
		x= (i & 1);
		y= (i & 2);
		z =(i & 4);
		_children[i].setLimits( (x ? v2[0] : _lowerBound[0]),
				(y ? v2[1] : _lowerBound[1]),
				(z ? v2[2] : _lowerBound[2]),
				(x ? _upperBound[0] : v2[0]),
				(y ? _upperBound[1] : v2[1]),
				(z ? _upperBound[2] : v2[2])
				);
	}

	_data.push_back(data);

	for (std::vector<AABBObject*>::const_iterator ptr=_data.begin();ptr!=_data.end();++ptr)
	{
		for (int i=0; i<8; ++i)
		{
		_children[i].addData(*ptr);
		}
	}

	_data.clear();

}

Octree::Node::~Node()
{
	if (_children != nullptr)
		delete [] _children;
}

Octree::~Octree()
{
	delete _root;
}


void Octree::Node::printSelf(std::ostream& os) const
{
	{

		if (_data.size()!=0)
		{
			os << "possible collisions";
			for (int i=0;i<_data.size();++i)
			{
			 os << _data[i];
			}
			os << std::endl;
		}
		if (_children!=nullptr)
		{
			for (int i=0;i<8;++i)
			_children[i].printSelf(os);
		}
	}

}

} // end namespace Geometry

} // end namespace SX
