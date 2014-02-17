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

#ifndef NSXTOOL_NDTREE_H_
#define NSXTOOL_NDTREE_H_
#include <cmath>
#include <set>
#include <vector>
#include <initializer_list>
#include <Eigen/Dense>
#include "AABB.h"


namespace SX {

namespace Geometry {

//! Typedef for unsigned integer
typedef unsigned int uint;

// Constant expression necessary to initialize
// _children attribute in the NDTree class
// which requires a constant literal.
constexpr int getPow (int factor)
{
	return factor > 1 ? 2 * getPow( factor-1 ) : 2;
}


/*! \brief A template class to handle binary trees in D dimensions
 *
 * NDTree is used to partition space in D dimensions and employed for fast sorting
 * or collision detections of AABB objects. NdTree derived from AABB since each
 * voxel is a bounded object itself. The root node has dimensions of the full world.
 * The NDTree instance does not own the AABB* objects: since users deals with
 * the lifetime of AABB* objects outside of the class, one must ensure that these
 * have a greater lifetime than that of NdTree. NDTree define a soft constraint on the
 * the maximum number of AABB* objects that can be stored in each leaf of the tree (_MAX_STORAGE),
 * and a maximal depth for each branch (_MAX_DEPTH). If the number of subdivisions reaches
 * _MAX_DEPTH, the soft constraint on _MAX_STORAGE is broken.
 *
*/

// ! Forward declaration of the iterator class
template<typename T, uint D> class NDTreeIterator;

template<typename T, uint D>
class NDTree : public AABB<T,D>
{
public:

	//! The NDTree iterator class is made friend with NDTree in order to access some of its private data
	friend class NDTreeIterator<T,D>;

	// These typedefs insure compatibility with STL
	typedef NDTreeIterator<T,D> iterator;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;
	typedef T value_type;
	typedef T * pointer;
	typedef T & reference;

	// ! A typedef for 1D vector
	typedef Eigen::Matrix<T,D,1> vector;

	//! Pair of AABB*
	typedef typename std::pair< AABB<T,D>*,AABB<T,D>* > collision_pair;

	//! Constructor from two Eigen3 vectors, throw invalid_argument if lb < ub
	NDTree(const vector& lb, const vector& ub);

	//! Constructor from two initializer lists, throw invalid_argument if lb< ub
	NDTree(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);

	//! destructor
	~NDTree();

	/*! Add a new AABB object to the deepest leaf.
	 * If the leaf has reached capacity of _MAX_STORAGE, then it will be split into
	 * 2^D sub-NDTree, unless _MAX_DEPTH is reached, in which case data will simply
	 * be added to this leaf.
	 */
	void addData(AABB<T,D>* aabb);

	//! Check whether the node has some children
	bool hasChildren() const;

	//! Check whether the node has some data
	bool hasData() const;

	//! Recursively send some information about the node (and children) to a stream
	void printSelf(std::ostream& os) const;

	//! Change the _MAX_DEPTH property of the Tree
	static void setMaxDepth(uint maxDepth);

	//! Change the _MAX_STORAGE property of the Tree
	static void setMaxStorage(uint maxStorage);

	//! Return the list of AABB* pairs that intercept.
	void getPossibleCollisions(std::set<collision_pair>& collisions) const;

	//! Get the voxels of the tree
	void getVoxels(std::vector<AABB<T,D>* >& voxels);

	//! Remove a data from the NDTree
	void removeData(const AABB<T,D>* data);

	iterator begin();

	iterator end();


private:
	//! Prevent defining tree with null world.
	NDTree();

	/*! Construct a new NDTree in the subregion i of
	 *  the parent tree. For example i goes from 1 to 8 for voxels
	 */
	NDTree(const NDTree* parent, uint i);

	//! Set all children to nullptr
	void nullifyChildren();

	//! Split the node into 2^D subnodes
	void split();

	//! Method to initialize vector of powers 2^D
	static std::vector<uint> createPowers();

	//! Maximum number of recursive splits of NDTree
	static uint _MAX_DEPTH;

	//! Maximum number of AABB* data object stored in each leaf
	static uint _MAX_STORAGE;

	//! Multiplicity of the tree branch (D=2 ->4), (D=3 ->8)
	static const uint _MULTIPLICITY;

	//! vector of powers 2^D
	static std::vector<uint> _POWERS;

	//! Vector of 2^D children
	NDTree<T,D>* _children[getPow(D)];

	//! Vector of data object in this leaf
	std::vector<AABB<T,D>*> _data;

	//! Depth of this branch with respect to root node.
	uint _depth;

	const NDTree<T,D>* _parent;

	const NDTree<T,D>* _right;

};

template<typename T, uint D>
std::vector<uint> NDTree<T,D>::createPowers()
{
	std::vector<uint> p(D);
	int i=0;
	// Powers of 2
	std::generate(p.begin(),
			      p.end(),
			      [&i]()
			      {
					return std::pow(2,i++);
			      }
	);

	return p;
}

template<typename T, uint D>
uint NDTree<T,D>::_MAX_DEPTH(10);

template<typename T, uint D>
uint NDTree<T,D>::_MAX_STORAGE(5);

template<typename T, uint D>
const uint NDTree<T,D>::_MULTIPLICITY(std::pow(2,D));

template<typename T, uint D>
std::vector<uint> NDTree<T,D>::_POWERS=std::move(createPowers());


template<typename T, uint D>
void NDTree<T,D>::nullifyChildren()
{
	for (uint i=0;i<_MULTIPLICITY;++i)
		_children[i]=nullptr;
}

template<typename T, uint D>
NDTree<T,D>::NDTree()
: AABB<T,D>(), _depth(0), _parent(nullptr), _right(nullptr)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);

}

template<typename T, uint D>
NDTree<T,D>::NDTree(const vector& lb, const vector& ub)
: AABB<T,D>(lb,ub), _depth(0), _parent(nullptr), _right(nullptr)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);
}

template<typename T, uint D>
NDTree<T,D>::NDTree(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub)
: AABB<T,D>(lb,ub), _depth(0), _parent(nullptr), _right(nullptr)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);
}

template<typename T, uint D>
NDTree<T,D>::NDTree(const NDTree<T,D>* parent, uint sector)
: AABB<T,D>(), _depth(parent->_depth+1), _parent(parent)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);

	if (sector == _MULTIPLICITY)
	{
		_right = nullptr;
	}
	else
	{
		_right = parent->_children[sector+1];
	}

	// Calculate the center of the current branch
	vector center = parent->getCenter();

	// The numbering of sub-voxels is encoded into bits of an int a follows:
	// ....... | dim[2] | dim[1] | dim[0]
	for (uint i=0; i<D; ++i)
	{
		bool b = (sector & _POWERS[i]);
		this->AABB<T,D>::_lowerBound(i) = (b ? center[i] : parent->AABB<T,D>::_lowerBound(i));
		this->AABB<T,D>::_upperBound(i) = (b ? parent->AABB<T,D>::_upperBound(i) : center(i));
	}

}

template<typename T, uint D>
NDTree<T,D>::~NDTree()
{
	if (hasChildren())
	{
		for (unsigned int i=0;i<_MULTIPLICITY;++i)
		{
			delete _children[i];
		}
	}

	return;
}

template<typename T, uint D>
void NDTree<T,D>::addData(AABB<T,D>* aabb)
{
	// AABB does not overlap with this branch
	if (!intercept(*aabb))
		return;

	// AABB overlap with this node
	if (hasChildren())
	{
		for (unsigned int i=0;i<_MULTIPLICITY;++i)
			_children[i]->addData(aabb);
	}
	else
	{
		_data.push_back(aabb);
		if (_data.size() > _MAX_STORAGE)
			split();
	}

}

template<typename T, uint D>
bool NDTree<T,D>::hasChildren() const
{
	return (_children[0]!=nullptr);
}

template<typename T, uint D>
bool NDTree<T,D>::hasData() const
{
	return (_data.size() != 0);
}

template<typename T, uint D>
void NDTree<T,D>::getPossibleCollisions(std::set<collision_pair >& collisions) const
{
	typedef std::set<std::pair<AABB<T,D>*,AABB<T,D>*> > setcol;

	if (hasData())
	{
		for (auto it1=_data.begin(); it1!=_data.end(); ++it1)
		{
			// First AABB
			AABB<T,D>* bb1=*it1;
			for (auto it2=it1+1; it2!=_data.end(); ++it2)
			{
				// Second AABB
				AABB<T,D>* bb2=*it2;
				// If two AABBs intersect, add to the set sorting the addresses
				if (bb1->AABB<T,D>::intercept(*bb2))
				{
					if (bb1 < bb2)
						collisions.insert(typename setcol::value_type(bb1,bb2));
					else
						collisions.insert(typename setcol::value_type(bb1,bb2));
				}
			}
		}
	}

	if (hasChildren())
	{
		for (uint i=0;i<_MULTIPLICITY;++i)
			_children[i]->getPossibleCollisions(collisions);
	}

	return;
}

template<typename T, uint D>
void NDTree<T,D>::getVoxels(std::vector<AABB<T,D>* >& voxels)
{
	voxels.push_back(this);
	if (hasChildren())
	{
		for (uint i=0;i<_MULTIPLICITY;++i)
			_children[i]->getVoxels(voxels);
	}
	return;
}

template<typename T, uint D>
void NDTree<T,D>::printSelf(std::ostream& os) const
{

	std::cout << "*** Node ***  " << this->_lowerBound  << "," << this->_upperBound << std::endl;
	if (!hasChildren())
	{
		std::cout << " has no children" <<std::endl;
		std::cout << "... and has " << _data.size() << " data" <<  std::endl;
	}
	else
	{
		std::cout << " has children :" << std::endl;
		for (int i=0; i<_MULTIPLICITY; ++i)
			_children[i]->printSelf(os);
	}
}

template<typename T, uint D>
void NDTree<T,D>::removeData(const AABB<T,D>* data)
{

	if (hasData())
	{
		auto it = std::find(_data.begin(), _data.end(), data);
		if (it!=_data.end())
			_data.erase(it);
	}

	if (hasChildren())
	{
		for (uint i=0; i<_MULTIPLICITY; ++i)
			_children[i]->removeData(data);
	}

}

template<typename T, uint D>
void NDTree<T,D>::setMaxDepth(uint maxDepth)
{
	if (maxDepth ==0)
		throw std::invalid_argument("Depth of the NDTree must be at least 1");
	if (maxDepth >10)
		throw std::invalid_argument("Depth of NDTree > 10 consume too much memory");

	_MAX_DEPTH = maxDepth;
}

template<typename T, uint D>
void NDTree<T,D>::setMaxStorage(uint maxStorage)
{
	if (maxStorage ==0)
		throw std::invalid_argument("MaxStorage of NDTree must be at least 1");

	_MAX_STORAGE = maxStorage;
}

template<typename T, uint D>
void NDTree<T,D>::split()
{
	// The node is already at the maximum depth: not allowed to split anymore.
	// Do nothing singe _data has already been added to parent node.
	if (_depth > _MAX_DEPTH)
		return;

	// Split the current node into 2^D subnodes
	for (uint i=0; i<_MULTIPLICITY; ++i)
		_children[i]=new NDTree<T,D>(this,i);

	for (auto ptr=_data.begin(); ptr!=_data.end(); ++ptr)
	{
		for (uint i=0; i<_MULTIPLICITY; ++i)
			_children[i]->addData(*ptr);
	}
	_data.clear();
}

template<typename T, uint D>
typename NDTree<T,D>::iterator NDTree<T,D>::begin()
{
	return iterator(*this);
}

template<typename T, uint D>
typename NDTree<T,D>::iterator NDTree<T,D>::end()
{
	return iterator();
}

template<typename T, uint D>
std::ostream& operator<<(std::ostream& os, const NDTree<T,D>& tree)
{
	tree.printSelf(os);
	return os;
}

template<typename T, uint D>
class NDTreeIterator
{
public:
	//; The default constructor. Used only for end condition.
	NDTreeIterator();

	//; The constructor from a reference to the NDTree to be iterated.
	NDTreeIterator(NDTree<T,D>& tree);

	NDTreeIterator<T,D>& operator=(const NDTreeIterator<T,D>& other);

	bool operator==(const NDTreeIterator<T,D>& other) const;

	bool operator!=(const NDTreeIterator<T,D>& other) const;

	const NDTree<T,D>& operator*() const;

	const NDTree<T,D>* operator->() const;

	NDTreeIterator<T,D>& operator++();

	NDTreeIterator<T,D>& operator++(int);

private:

	const NDTree<T,D>* _node;

};

template<typename T, uint D>
NDTreeIterator<T,D>::NDTreeIterator() : _node(nullptr)
{
}

template<typename T, uint D>
NDTreeIterator<T,D>::NDTreeIterator(NDTree<T,D>& node) : _node(&node)
{
}

template<typename T, uint D>
NDTreeIterator<T,D>& NDTreeIterator<T,D>::operator=(const NDTreeIterator<T,D>& other)
{
  if (this != &other)
    {
	  _node = other._node;
    }
  return *this;
}

template<typename T, uint D>
bool NDTreeIterator<T,D>::operator!=(const NDTreeIterator<T,D>& other) const
{
	return (_node != other._node);
}

template<typename T, uint D>
bool NDTreeIterator<T,D>::operator==(const NDTreeIterator<T,D>& other) const
{
	return (_node == other._node);
}

template<typename T, uint D>
const NDTree<T,D>& NDTreeIterator<T,D>::operator*() const
{
	return *_node;
}

template<typename T, uint D>
const NDTree<T,D>* NDTreeIterator<T,D>::operator->() const
{
	return _node;
}

template<typename T, uint D>
NDTreeIterator<T,D>& NDTreeIterator<T,D>::operator++()
{
	if (_node->hasChildren())
	{
		_node = _node->_children[0];
	}
	else
	{
		while (_node->_right == nullptr)
		{
			if (_node->_parent == nullptr)
				break;
			_node = _node->_parent;
		}
		_node = _node->_right;
	}
	return (*this);
}

template<typename T, uint D>
NDTreeIterator<T,D>& NDTreeIterator<T,D>::operator++(int)
{
	NDTreeIterator<T,D> tmp(*this);
	++(*this);
	return (tmp);
}

} // namespace Geometry

} // namespace SX

#endif /* NSXTOOL_NDTREE_H_ */
