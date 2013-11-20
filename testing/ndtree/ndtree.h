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

#ifndef NSXTOOL_NDTree_H_
#define NSXTOOL_NDTree_H_

#include <vector>
#include <boost/numeric/ublas/vector.hpp>

#include "aabb.h"

using namespace boost::numeric::ublas;

namespace SX {

namespace Geometry {

template<typename T, std::size_t D>
class NDTree
{
public:

	typedef std::pair< std::vector<AABB<T,D>*>::iterator , std::vector<AABB<T,D>*>::iterator > data_range_iterator;

	class Node : public AABB<T,D>
	{
	public:

		//! default constructor
		Node();

		//! constructor from two ublas vectors
		Node(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub);

		//! destructor
		~Node();

		//! add a new AABB object to the node
		void addData(AABB<T,D>* data);

		//! get the data bounded to a node (and its descendance)
		void getData(std::vector<data_range_iterator>& treeData) const;

		//! check whether the node has some children
		bool hasChildren() const;

		//! check whether the node has some children
		bool hasData() const;

		//! recursively send some information about the node (and its descendance) to a stream
		void printSelf(std::ostream& os) const;

		//! setter for _depth attribute
		void setDepth(std::size_t depth);

		//! split the node into 2^D children
		void split(AABB<T,D>* data);

	private:
		std::vector<AABB<T,D>*> _data;
		Node* _children;
		std::size_t _depth;

	};

	//! constructor from two ublas vectors
	NDTree(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub);

	//! destructor
	~NDTree();

	//! define the tree parameters
	static void defineParameters(std::size_t maxStorage, std::size_t maxDepth);

	//! add a new AABB to the tree
	void addData(AABB<T,D>* data);

	//! Get the data bounded to all the nodes of the tree
	std::vector<data_range_iterator> getData() const;

	//! getter for the root node
	NDTree<T,D>::Node* getRoot();

	//! send some information about the tree to a stream
	void printSelf(std::ostream& os) const;


public:
	static int _nSplits;

private:
	static std::vector<std::size_t> createPowers();
	static std::size_t _maxStorage;
	static std::size_t _maxDepth;
	static std::size_t _multiplicity;
	static std::vector<std::size_t> _powers;
	Node* _root;
};

template<typename T, std::size_t D>
int NDTree<T,D>::_nSplits(0);

template<typename T, std::size_t D>
std::size_t NDTree<T,D>::_maxStorage(2);

template<typename T, std::size_t D>
std::size_t NDTree<T,D>::_maxDepth(10);

template<typename T, std::size_t D>
std::size_t NDTree<T,D>::_multiplicity(std::pow(2,D));

template<typename T, std::size_t D>
std::vector<std::size_t> NDTree<T,D>::_powers=createPowers();

template<typename T, std::size_t D>
NDTree<T,D>::NDTree(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub)
{
	_root = new Node(lb, ub);
}

template<typename T, std::size_t D>
NDTree<T,D>::~NDTree()
{
	delete _root;
}

template<typename T, std::size_t D>
void NDTree<T,D>::addData(AABB<T,D>* data)
{
	_root->addData(data);
}

template<typename T, std::size_t D>
std::vector<std::size_t> NDTree<T,D>::createPowers()
{
	std::vector<std::size_t> p(D);
	int i=0;
	std::generate(p.begin(), p.end(), [&i]() {return std::pow(2,i++);});
	return p;
}

template<typename T, std::size_t D>
void NDTree<T,D>::defineParameters(std::size_t maxStorage, std::size_t maxDepth)
{
	_maxStorage = maxStorage;
	_maxDepth = maxDepth;
}

template<typename T, std::size_t D>
std::vector<NDTree<T,D>::data_range_iterator> NDTree<T,D>::getData() const
{
	std::vector<NDTree<T,D>::data_range_iterator> tmp;
	_root->getData(tmp);
	return tmp;
}

template<typename T, std::size_t D>
typename NDTree<T,D>::Node* NDTree<T,D>::getRoot()
{
	return _root;
}

template<typename T, std::size_t D>
void NDTree<T,D>::printSelf(std::ostream& os) const
{
	_root->printSelf(os);
}

// inner class methods definitions

template<typename T, std::size_t D>
NDTree<T,D>::Node::Node() : AABB<T,D>(), _children(nullptr), _depth(0)
{
	_data.reserve(_maxStorage);
}


template<typename T, std::size_t D>
NDTree<T,D>::Node::Node(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub) : AABB<T,D>(lb,ub), _children(nullptr), _depth(0)
{
	_data.reserve(_maxStorage);
}

template<typename T, std::size_t D>
NDTree<T,D>::Node::~Node()
{
	delete [] _children;
}

template<typename T, std::size_t D>
void NDTree<T,D>::Node::addData(AABB<T,D>* data)
{
	if (!intercept(*data))
		return;

	if (hasChildren())
	{
		for (std::size_t i=0; i<_multiplicity; ++i)
			_children[i].addData(data);
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

template<typename T, std::size_t D>
void NDTree<T,D>::Node::getData(std::vector<NDTree<T,D>::data_range_iterator>& treeData) const
{

	if (_children!=nullptr)
	{
		for (int i=0; i<_multiplicity; ++i)
			_children[i].getData(treeData);
	}
	else
	{
		if (_data.size() != 0)
		{
			treeData.push_back(std::pair<NDTree<T,D>::data_range_iterator,NDTree<T,D>::data_range_iterator>(_data.begin(),_data.end()));
		}
	}
}

template<typename T, std::size_t D>
bool NDTree<T,D>::Node::hasChildren() const
{
	return (_children != nullptr);
}

template<typename T, std::size_t D>
bool NDTree<T,D>::Node::hasData() const
{
	return (_data.size() != 0);
}

template<typename T, std::size_t D>
void NDTree<T,D>::Node::setDepth(std::size_t depth)
{
	_depth = depth;
}

template<typename T, std::size_t D>
void NDTree<T,D>::Node::split(AABB<T,D>* data)
{

	// The node is already at the maximum depth: do not split anymore, just add the data
	if (_depth > _maxDepth)
	{
		_data.push_back(data);
		return;
	}

	_nSplits++;

	_children = new Node[_multiplicity];

	bounded_vector<T,D> center((this->_upperBound + this->_lowerBound)*0.5);

	// The lower and upper limits for each splitted node.
	bounded_vector<T,D> lL, uL;

	bool b;
	for (std::size_t i=0; i<_multiplicity; ++i)
	{
		for (std::size_t j=0; j<D; ++j)
		{
			b = (i & _powers[j]);
			lL[j] = (b ? center[j] : this->_lowerBound[j]);
			uL[j] = (b ? this->_upperBound[j] : center[j]);
		}
		_children[i].setBounds(lL,uL);
	}

	_data.push_back(data);
	for (auto ptr=_data.begin(); ptr!=_data.end(); ++ptr)
	{
		for (std::size_t i=0; i<_multiplicity; ++i)
		{
			_children[i].addData(*ptr);
			_children[i].setDepth(_depth+1);
		}
	}

	_data.clear();

}

template<typename T, std::size_t D>
void NDTree<T,D>::Node::printSelf(std::ostream& os) const
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
		for (int i=0; i<_multiplicity; ++i)
			_children[i].printSelf(os);
	}
}

} // namespace Geometry

} // namespace SX

#endif /* NSXTOOL_NDTree_H_ */
