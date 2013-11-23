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
#include <boost/numeric/ublas/vector.hpp>
#include "AABB.h"


namespace SX {

namespace Geometry {


namespace ublas=boost::numeric::ublas;


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
template<typename T, std::size_t D>
class NDTree : public AABB<T,D>
{
public:

	typedef typename std::vector<AABB<T,D>*>::const_iterator data_iterator;
	typedef std::pair< data_iterator , data_iterator > data_range_pair;
	typedef typename std::pair< AABB<T,D>*,AABB<T,D>* > collision_pair;


	//! Constructor from two ublas vectors, throw invalid_argument if lb < ub
	NDTree(const ublas::bounded_vector<T,D>& lb, const ublas::bounded_vector<T,D>& ub);

	//! Constructor from two initializer lists, throw invalid_argument if lb< ub
	NDTree(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);

	/*! Constructor from a parent node
	 *
	 */
	NDTree(const NDTree* parent, std::size_t nd_dran);

	//! destructor
	~NDTree();

	/*! Add a new AABB object to the deepest leaf.
	 *
	 */
	void addData(AABB<T,D>* aabb);

	//! get the data contained in the node
	void getData(std::vector<NDTree<T,D>::data_range_pair>& treeData) const;

	//! check whether the node has some children
	bool hasChildren() const;

	//! check whether the node has some data
	bool hasData() const;

	//! recursively send some information about the node (and its descendance) to a stream
	void printSelf(std::ostream& os) const;

	void collisions(int& ) const;

	//! setter for _MAX_DEPTH attribute
	static void setDepth(std::size_t depth);

	//! setter for _MAX_STORAGE attribute
	static void setMaxStorage(std::size_t maxStorage);

	void getPossibleCollisions(std::set< NDTree<T,D>::collision_pair >& collisions) const;

	//! split the node into 2^D subnodes
	void split();

private:
	//! Prevent defining tree with null world.
	NDTree();

	//! Set all children to nullptr
	void nullifyChildren();

	//! Method to initialize vector of powers 2^D
	static std::vector<std::size_t> createPowers();

	//! Maximum number of recursive splits of NDTree
	static std::size_t _MAX_DEPTH;

	//! Maximum number of AABB* data object stored in each leaf
	static std::size_t _MAX_STORAGE;

	//! Multiplicity of the tree branch (D=2 ->4), (D=3 ->8)
	static const std::size_t _MULTIPLICITY;

	//! vector of powers 2^D
	static std::vector<std::size_t> _POWERS;

	//! Vector of 2^D children
	NDTree<T,D>* _children[getPow(D)];

	//! Vector of data object in this leaf
	std::vector<AABB<T,D>*> _data;

	//! Depth of this branch with respect to root node.
	std::size_t _depth;

};

template<typename T, std::size_t D>
std::vector<std::size_t> NDTree<T,D>::createPowers()
{
	std::vector<std::size_t> p(D);
	int i=0;
	std::generate(p.begin(), p.end(), [&i]() {return std::pow(2,i++);});

	return p;
}

template<typename T, std::size_t D>
std::size_t NDTree<T,D>::_MAX_DEPTH(10);

template<typename T, std::size_t D>
std::size_t NDTree<T,D>::_MAX_STORAGE(5);

template<typename T, std::size_t D>
const std::size_t NDTree<T,D>::_MULTIPLICITY(std::pow(2,D));

template<typename T, std::size_t D>
std::vector<std::size_t> NDTree<T,D>::_POWERS=std::move(createPowers());


template<typename T, std::size_t D>
void NDTree<T,D>::nullifyChildren()
{
	for (std::size_t i=0;i<_MULTIPLICITY;++i)
		_children[i]=nullptr;
}

template<typename T, std::size_t D>
NDTree<T,D>::NDTree() : AABB<T,D>(), _depth(0)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);

}

template<typename T, std::size_t D>
NDTree<T,D>::NDTree(const ublas::bounded_vector<T,D>& lb, const ublas::bounded_vector<T,D>& ub)
: AABB<T,D>(lb,ub), _depth(0)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);
}

template<typename T, std::size_t D>
NDTree<T,D>::NDTree(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub)
: AABB<T,D>(lb,ub), _depth(0)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);
}

template<typename T, std::size_t D>
NDTree<T,D>::NDTree(const NDTree<T,D>* parent, std::size_t nd_dran)
: AABB<T,D>(parent->_lowerBound, parent->_upperBound), _depth(parent->_depth+1)
{
	nullifyChildren();
	_data.reserve(_MAX_STORAGE);

	// Calculate the center of the current branch
	ublas::bounded_vector<T,D> center((parent->AABB<T,D>::_lowerBound + parent->AABB<T,D>::_upperBound)*0.5);

	// The numbering of sub-voxels is encoded into bits of an int a follows:
	// ....... | dim[2] | dim[1] | dim[0]
	for (std::size_t i=0; i<D; ++i)
	{
		bool b = (nd_dran & _POWERS[i]);
		this->AABB<T,D>::_lowerBound[i] = (b ? center[i] : parent->AABB<T,D>::_lowerBound[i]);
		this->AABB<T,D>::_upperBound[i] = (b ? parent->AABB<T,D>::_upperBound[i] : center[i]);
	}

}

template<typename T, std::size_t D>
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

template<typename T, std::size_t D>
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
		if (_data.size() >= _MAX_STORAGE)
			split();
	}

}

template<typename T, std::size_t D>
void NDTree<T,D>::getData(std::vector<NDTree<T,D>::data_range_pair>& treeData) const
{
	if (hasChildren())
	{
		for (unsigned int i=0; i<_MULTIPLICITY; ++i)
			_children[i]->getData(treeData);
	}
	else
	{
		if (_data.size() != 0)
			treeData.push_back(std::make_pair(_data.begin(),_data.end()));
	}
}

template<typename T, std::size_t D>
bool NDTree<T,D>::hasChildren() const
{
	return (_children[0]!=nullptr);
}

template<typename T, std::size_t D>
bool NDTree<T,D>::hasData() const
{
	return (_data.size() != 0);
}


template<typename T, std::size_t D>
void NDTree<T,D>::collisions(int& col) const
{

	if (!hasChildren() && hasData())
	{
		col+=_data.size()*(_data.size()-1)/2;
	}
	else if (hasChildren())
	{
		for (int i=0;i<_MULTIPLICITY;++i)
			_children[i]->collisions(col);
	}
}

template<typename T, std::size_t D>
void NDTree<T,D>::getPossibleCollisions(std::set<collision_pair >& collisions) const
{
	typedef std::set<std::pair<AABB<T,D>*,AABB<T,D>*> > setcol;

	if (!hasChildren() && hasData())
	{
		for (auto it1=_data.begin(); it1!=_data.end(); ++it1)
		{
			for (auto it2=it1+1; it2!=_data.end(); ++it2)
			{
				if ((*(it1))->AABB<T,D>::intercept(*(*it2)))
				{
					if (&(*it1) < &(*it2))
					{
						collisions.insert(typename setcol::value_type(*it1,*it2));
					}
					else
					{
						collisions.insert(typename setcol::value_type(*it2,*it1));
					}
				}
			}
		}

	}
	else if (hasChildren())
	{
		for (int i=0;i<_MULTIPLICITY;++i)
			_children[i]->getPossibleCollisions(collisions);
	}
}

template<typename T, std::size_t D>
void NDTree<T,D>::printSelf(std::ostream& os) const
{

	std::cout << "*** Node ***  " << this->_lowerBound  << "," << this->_upperBound << std::endl;
	if (!hasChildren())
	{
		std::cout << " has no children" <<std::endl;
		std::cout << "... and has " << _data.size() << "data" <<  std::endl;
	}
	else
	{
		std::cout << " has children :" << std::endl;
		for (int i=0; i<_MULTIPLICITY; ++i)
			_children[i]->printSelf(os);
	}
}

template<typename T, std::size_t D>
void NDTree<T,D>::setDepth(std::size_t depth)
{
	if (depth ==0)
		throw std::invalid_argument("Depth of the NDTree must be at least 1");
	if (depth >10)
		throw std::invalid_argument("Depth of NDTree > 10 consume too much memory");

	_MAX_DEPTH = depth;
}

template<typename T, std::size_t D>
void NDTree<T,D>::setMaxStorage(std::size_t maxStorage)
{
	if (maxStorage ==0)
		throw std::invalid_argument("MaxStorage of NDTree must be at least 1");

	_MAX_STORAGE = maxStorage;
}

template<typename T, std::size_t D>
void NDTree<T,D>::split()
{
	// The node is already at the maximum depth: not allowed to split anymore.
	// Do nothing singe _data has already been added to parent node.
	if (_depth > _MAX_DEPTH)
		return;

	// Split the current node into 2^D subnodes
	for (std::size_t i=0; i<_MULTIPLICITY; ++i)
		_children[i]=new NDTree<T,D>(this,i);

	for (auto ptr=_data.begin(); ptr!=_data.end(); ++ptr)
	{
		for (std::size_t i=0; i<_MULTIPLICITY; ++i)
			_children[i]->addData(*ptr);
	}
	_data.clear();
}

template<typename T, std::size_t D>
std::ostream& operator<<(std::ostream& os, const NDTree<T,D>& tree)
{
	tree.printSelf(os);
	return os;
}

} // namespace Geometry

} // namespace SX

#endif /* NSXTOOL_NDTREE_H_ */
