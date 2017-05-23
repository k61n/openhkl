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

#ifndef NSXLIB_NDTREE_H
#define NSXLIB_NDTREE_H

#include <cmath>
#include <initializer_list>
#include <set>
#include <vector>

#include <Eigen/Dense>

#include "AABB.h"

namespace nsx {

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
template<typename T, unsigned int D> class NDTreeIterator;

template<typename T, unsigned int D>
class NDTree : public AABB {
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

    //! Pair of AABB*
    typedef typename std::pair<const IShape*, const IShape* > collision_pair;

    //! Move constructor
    NDTree(NDTree<T,D>&& other);

    //! Copy constructor
    NDTree(const NDTree<T,D>& other) = delete;

    //! Assignment operator
    NDTree<T,D>& operator=(const NDTree<T,D>& other) = delete;

    //! Constructor from two Eigen3 vectors, throw invalid_argument if lb < ub
    NDTree(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub);

    //! Constructor from two initializer lists, throw invalid_argument if lb< ub
    NDTree(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);

    //! destructor
    ~NDTree();

    //IShape<T,D>* clone() const;

    /*! Add a new AABB object to the deepest leaf.
     * If the leaf has reached capacity of _MAX_STORAGE, then it will be split into
     * 2^D sub-NDTree, unless _MAX_DEPTH is reached, in which case data will simply
     * be added to this leaf.
     */
    void addData(const IShape* aabb);

    //! Check whether the node has some children
    bool hasChildren() const;

    //! Check whether the node has some data
    bool hasData() const;

    //! Recursively send some information about the node (and children) to a stream
    void printSelf(std::ostream& os) const;

    //! Change the _MAX_DEPTH property of the Tree
    void setMaxDepth(unsigned int maxDepth);

    //! Change the _MAX_STORAGE property of the Tree
    void setMaxStorage(unsigned int maxStorage);

    //! Return the list of AABB* pairs that intercept.
    //void getPossibleCollisions(std::set<collision_pair>& collisions) const;
    std::set<collision_pair> getCollisions() const;

    //! Return collisions with a given shape
    std::set<const IShape*> getCollisions(const IShape& given) const;

    //! Return true if the point is contained in any object of the octree
    bool isInsideObject(const HomVector& vector);

    //! Get the voxels of the tree
    void getVoxels(std::vector<AABB* >& voxels);

    //! Remove a data from the NDTree
    void removeData(const IShape* data);

    iterator begin() const;

    iterator end() const;

    const std::vector<const IShape*>& getData() const {return _data;}

    NDTree(const NDTree* parent, unsigned int i);

    unsigned int numChambers() const;


private:
    //! Prevent defining tree with null world.
    NDTree();

    /*! Construct a new NDTree in the subregion i of
     *  the parent tree. For example i goes from 1 to 8 for voxels
     */

    //! Set all children to nullptr
    void nullifyChildren();

    //! Split the node into 2^D subnodes
    void split();

    //! Method to initialize vector of powers 2^D
    static std::vector<unsigned int> createPowers();

    //! Maximum number of recursive splits of NDTree
    unsigned int _MAX_DEPTH = 5;

    //! Maximum number of AABB* data object stored in each leaf
    unsigned int _MAX_STORAGE = 5;

    //! Multiplicity of the tree branch (D=2 ->4), (D=3 ->8)
    const unsigned int _MULTIPLICITY = std::pow(2,D);

    //! vector of powers 2^D
    std::vector<unsigned int> _POWERS = createPowers();

    //! Vector of 2^D children
    std::vector<NDTree<T,D>> _children;
    // NDTree<T,D>* _children[getPow(D)];

    //! Vector of data object in this leaf
    std::vector<const IShape*> _data;

    //! Depth of this branch with respect to root node.
    unsigned int _depth;

    const NDTree<T,D>* _parent;
    long _idx = -1;
};

template<typename T, unsigned int D>
std::vector<unsigned int> NDTree<T,D>::createPowers()
{
    std::vector<unsigned int> p(D);
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

template<typename T, unsigned int D>
NDTree<T, D>::NDTree(NDTree<T,D>&& other):
    _MAX_DEPTH(other._MAX_DEPTH),
    _MAX_STORAGE(other._MAX_STORAGE),
    _MULTIPLICITY(other._MULTIPLICITY),
    _POWERS(std::move(other._POWERS)),
    _children(std::move(other._children)),
    _data(std::move(other._data)),
    _depth(other._depth),
    _parent(other._parent),
    _idx(other._idx)
{

}

template<typename T, unsigned int D>
void NDTree<T,D>::nullifyChildren()
{
    _children.clear();
}

template<typename T, unsigned int D>
NDTree<T,D>::NDTree():
    AABB(), _depth(0), _parent(nullptr)//, _right(nullptr)
{
    nullifyChildren();
    //_data.reserve(_MAX_STORAGE);
}

template<typename T, unsigned int D>
NDTree<T,D>::NDTree(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub):
    AABB(lb,ub), _depth(0), _parent(nullptr)//, _right(nullptr)
{
    nullifyChildren();
    //_data.reserve(_MAX_STORAGE);
}

template<typename T, unsigned int D>
NDTree<T,D>::NDTree(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub):
    AABB(lb,ub), _depth(0), _parent(nullptr)//, _right(nullptr)
{
    nullifyChildren();
    //_data.reserve(_MAX_STORAGE);
}

template<typename T, unsigned int D>
NDTree<T,D>::NDTree(const NDTree<T,D>* parent, unsigned int sector):
    AABB(), _depth(parent->_depth+1), _parent(parent), _idx(sector),
    _MAX_DEPTH(parent->_MAX_DEPTH),
    _MAX_STORAGE(parent->_MAX_STORAGE)
{
    nullifyChildren();

    // Calculate the center of the current branch
    Eigen::Vector3d center = parent->getAABBCenter();

    // The numbering of sub-voxels is encoded into bits of an int a follows:
    // ....... | dim[2] | dim[1] | dim[0]
    for (unsigned int i=0; i<D; ++i) {
        bool b = (sector & _POWERS[i]);
        this->AABB::_lowerBound(i) = (b ? center[i] : parent->AABB::_lowerBound(i));
        this->AABB::_upperBound(i) = (b ? parent->AABB::_upperBound(i) : center(i));
    }
}

template<typename T, unsigned int D>
NDTree<T,D>::~NDTree()
{
}

template<typename T, unsigned int D>
void NDTree<T,D>::addData(const IShape* shape)
{
    // AABB does not overlap with this branch
    if (!this->intercept(*shape)) {
        return;
    }

    // AABB overlap with this node
    if (hasChildren()) {
        for (auto&& child: _children) {
            child.addData(shape);
        }
    } else {
        _data.push_back(shape);
        if (_data.size() > _MAX_STORAGE) {
            split();
        }
    }
}

template<typename T, unsigned int D>
bool NDTree<T,D>::hasChildren() const
{
    return (!_children.empty());
}

template<typename T, unsigned int D>
bool NDTree<T,D>::hasData() const
{
    return (_data.size() != 0);
}

template<typename T, unsigned int D>
std::set<typename NDTree<T,D>::collision_pair> NDTree<T,D>::getCollisions() const
{
    std::set<collision_pair> collisions;

    // loop over chambers of the ndtree
    for (auto&& chamber: *this) {
        // loop over shapes in the chamber
        for (auto i = 0; i < chamber._data.size(); ++i) {
            for (auto j = i+1; j < chamber._data.size(); ++j) {
                auto&& a = chamber._data[i];
                auto&& b = chamber._data[j];

                // collision detected
                if (a->collide(*b)) {
                    if (a < b) {
                        collisions.emplace(collision_pair(a, b));
                    }
                    else {
                        collisions.emplace(collision_pair(b, a));
                    }
                }
            }
        }
    }
    return collisions;
}

template<typename T, unsigned int D>
std::set<const IShape*> NDTree<T,D>::getCollisions(const IShape& given) const
{
    using ndtree = NDTree<T, D>;
    using collision_set = std::set<const IShape*>;
    collision_set collisions;

    std::function<void(const ndtree*, collision_set&)> recursiveCollisions;

    recursiveCollisions = [&given, &recursiveCollisions] (const ndtree* tree, collision_set& collisions) -> void
    {
        // shape's box does not intercept tree
        if (!tree->intercept(given)) {
            return;
        }

        // tree has children
        if (tree->hasChildren()) {
            for (auto&& child: tree->_children) {
                recursiveCollisions(&child, collisions);
            }
            return;
        }

        // otherwise, tree has no children
        for (auto&& shape: tree->_data) {
            if (shape->collide(given)) {
                collisions.emplace(shape);
            }
        }
    };

    recursiveCollisions(this, collisions);
    return collisions;
}

template<typename T, unsigned int D>
bool NDTree<T,D>::isInsideObject(const HomVector& vector)
{
    // shape's box does not intercept tree
    if (!isInside(vector)) {
        return false;
    }

    // tree has children
    if (hasChildren()) {
        for (auto&& child: _children) {
            if (child.isInsideObject(vector)) {
                return true;
            }
        }
        return false;
    }

    // otherwise, tree has no children
    for (auto&& shape: _data) {
        if (shape->isInside(vector)) {
            return true;
        }
    }

    // no collision found
    return false;
}

template<typename T, unsigned int D>
void NDTree<T,D>::getVoxels(std::vector<AABB* >& voxels)
{
    voxels.push_back(this);
    if (hasChildren()) {
        for (unsigned int i=0;i<_MULTIPLICITY;++i) {
            _children[i]->getVoxels(voxels);
        }
    }
}

template<typename T, unsigned int D>
void NDTree<T,D>::printSelf(std::ostream& os) const
{
    os << "*** Node ***  " << this->_lowerBound  << "," << this->_upperBound << std::endl;
    if (!hasChildren()) {
        os << " has no children" <<std::endl;
        os << "... and has " << _data.size() << " data" <<  std::endl;
    } else {
        os << " has children :" << std::endl;
        for (int i = 0; i < _MULTIPLICITY; ++i) {
            _children[i]->printSelf(os);
        }
    }
}

template<typename T, unsigned int D>
void NDTree<T,D>::removeData(const IShape* data)
{
    if (hasData()) {
        auto it = std::find(_data.begin(), _data.end(), data);
        if (it!=_data.end()) {
            _data.erase(it);
        }
    }
    if (hasChildren())  {
        for (unsigned int i=0; i<_MULTIPLICITY; ++i) {
            _children[i].removeData(data);
        }
    }
}

template<typename T, unsigned int D>
void NDTree<T,D>::setMaxDepth(unsigned int maxDepth)
{
    if (maxDepth ==0) {
        throw std::invalid_argument("Depth of the NDTree must be at least 1");
    }
    if (maxDepth >10) {
        throw std::invalid_argument("Depth of NDTree > 10 consume too much memory");
    }
    _MAX_DEPTH = maxDepth;
}

template<typename T, unsigned int D>
void NDTree<T,D>::setMaxStorage(unsigned int maxStorage)
{
    if (maxStorage == 0) {
        throw std::invalid_argument("MaxStorage of NDTree must be at least 1");
    }
    _MAX_STORAGE = maxStorage;
}

template<typename T, unsigned int D>
void NDTree<T,D>::split()
{
    // The node is already at the maximum depth: not allowed to split anymore.
    // Do nothing singe _data has already been added to parent node.
    if (_depth > _MAX_DEPTH) {
        return;
    }

    _children.clear();
    _children.reserve(_MULTIPLICITY);

    // Split the current node into 2^D subnodes
    for (unsigned int i=0; i<_MULTIPLICITY; ++i) {
        _children.emplace_back(this, i);
    }
    for (auto ptr=_data.begin(); ptr!=_data.end(); ++ptr) {
        for (auto&& child: _children) {
            child.addData(*ptr);
        }
    }
    _data.clear();
}

template<typename T, unsigned int D>
typename NDTree<T,D>::iterator NDTree<T,D>::begin() const
{
    return iterator(*this);
}

template<typename T, unsigned int D>
typename NDTree<T,D>::iterator NDTree<T,D>::end() const
{
    return iterator();
}

template<typename T, unsigned int D>
std::ostream& operator<<(std::ostream& os, const NDTree<T,D>& tree)
{
    tree.printSelf(os);
    return os;
}

template<typename T, unsigned int D>
class NDTreeIterator
{
public:
    //; The default constructor. Used only for end condition.
    NDTreeIterator();

    //; The constructor from a reference to the NDTree to be iterated.
    NDTreeIterator(const NDTree<T,D>& tree);

    NDTreeIterator<T,D>& operator=(const NDTreeIterator<T,D>& other);

    bool operator==(const NDTreeIterator<T,D>& other) const;

    bool operator!=(const NDTreeIterator<T,D>& other) const;

    const NDTree<T,D>& operator*() const;

    const NDTree<T,D>* operator->() const;

    NDTreeIterator<T,D>& operator++();

    NDTreeIterator<T,D> operator++(int);

private:

    const NDTree<T,D>* _node;
};

template<typename T, unsigned int D>
NDTreeIterator<T,D>::NDTreeIterator() : _node(nullptr)
{
}

template<typename T, unsigned int D>
NDTreeIterator<T,D>::NDTreeIterator(const NDTree<T,D>& node) : _node(&node)
{
    // find the leftmost leaf
    while(_node->hasChildren()) {
        _node = &_node->_children[0];
    }
}

template<typename T, unsigned int D>
NDTreeIterator<T,D>& NDTreeIterator<T,D>::operator=(const NDTreeIterator<T,D>& other)
{
  _node = other._node;
  return *this;
}

template<typename T, unsigned int D>
bool NDTreeIterator<T,D>::operator!=(const NDTreeIterator<T,D>& other) const
{
    return (_node != other._node);
}

template<typename T, unsigned int D>
bool NDTreeIterator<T,D>::operator==(const NDTreeIterator<T,D>& other) const
{
    return (_node == other._node);
}

template<typename T, unsigned int D>
const NDTree<T,D>& NDTreeIterator<T,D>::operator*() const
{
    return *_node;
}

template<typename T, unsigned int D>
const NDTree<T,D>* NDTreeIterator<T,D>::operator->() const
{
    return _node;
}

template<typename T, unsigned int D>
NDTreeIterator<T,D>& NDTreeIterator<T,D>::operator++()
{
    // already at end
    if (_node == nullptr || _node->_parent == nullptr) {
        _node = nullptr;
        return *this;
    }

    // can move right
    if (_node->_idx < _node->_parent->_children.size()-1) {
        _node = &_node->_parent->_children[_node->_idx+1];

        while(_node->hasChildren()) {
            _node = &_node->_children[0];
        }

        return *this;
    }

    // can't move right: move up
    _node = _node->_parent;

    // reached the root: at end
    if (_node && _node->_parent == nullptr) {
        _node = nullptr;
    }

    return ++(*this);
}

template<typename T, unsigned int D>
NDTreeIterator<T,D> NDTreeIterator<T,D>::operator++(int)
{
    NDTreeIterator<T,D> tmp(*this);
    ++(*this);
    return (tmp);
}

template<typename T, unsigned int D>
unsigned int NDTree<T,D>::numChambers() const
{
    if (hasChildren()) {
        unsigned int count = 0;
        for (auto&& child: _children) {
            count += child.numChambers();
        }
        return count;
    }
    return 1;
}

} // end namespace nsx

#endif // NSXLIB_NDTREE_H
