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

#include "NDTree.h"

namespace nsx {

std::vector<unsigned int> NDTree::createPowers()
{
    std::vector<unsigned int> p(3);
    int i=0;
    // Powers of 2
    std::generate(p.begin(),p.end(),[&i](){return std::pow(2,i++);});

    return p;
}

NDTree::NDTree(NDTree&& other):
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

void NDTree::nullifyChildren()
{
    _children.clear();
}

NDTree::NDTree():
    AABB(), _depth(0), _parent(nullptr)
{
    nullifyChildren();
}

NDTree::NDTree(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub)
: AABB(lb,ub), 
  _depth(0), 
  _parent(nullptr)
{
    nullifyChildren();
}

NDTree::NDTree(const std::initializer_list<double>& lb, const std::initializer_list<double>& ub):
    AABB(lb,ub), _depth(0), _parent(nullptr)
{
    nullifyChildren();
}

NDTree::NDTree(const NDTree* parent, unsigned int sector):
    AABB(), _depth(parent->_depth+1), _parent(parent), _idx(sector),
    _MAX_DEPTH(parent->_MAX_DEPTH),
    _MAX_STORAGE(parent->_MAX_STORAGE)
{
    nullifyChildren();

    // Calculate the center of the current branch
    Eigen::Vector3d center = parent->getAABBCenter();

    // The numbering of sub-voxels is encoded into bits of an int a follows:
    // ....... | dim[2] | dim[1] | dim[0]
    for (unsigned int i=0; i<3; ++i) {
        bool b = (sector & _POWERS[i]);
        this->AABB::_lowerBound(i) = (b ? center[i] : parent->AABB::_lowerBound(i));
        this->AABB::_upperBound(i) = (b ? parent->AABB::_upperBound(i) : center(i));
    }
}

void NDTree::addData(const IShape* shape)
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

bool NDTree::hasChildren() const
{
    return (!_children.empty());
}

bool NDTree::hasData() const
{
    return (_data.size() != 0);
}

std::set<NDTree::collision_pair> NDTree::getCollisions() const
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

std::set<const IShape*> NDTree::getCollisions(const IShape& given) const
{
    using collision_set = std::set<const IShape*>;
    collision_set collisions;

    std::function<void(const NDTree*, collision_set&)> recursiveCollisions;

    recursiveCollisions = [&given, &recursiveCollisions] (const NDTree* tree, collision_set& collisions) -> void
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

bool NDTree::isInsideObject(const HomVector& vector)
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

void NDTree::getVoxels(std::vector<AABB*>& voxels)
{
    voxels.push_back(this);
    if (hasChildren()) {
        for (unsigned int i=0;i<_MULTIPLICITY;++i) {
            _children[i].getVoxels(voxels);
        }
    }
}

void NDTree::printSelf(std::ostream& os) const
{
    os << "*** Node ***  " << this->_lowerBound  << "," << this->_upperBound << std::endl;
    if (!hasChildren()) {
        os << " has no children" <<std::endl;
        os << "... and has " << _data.size() << " data" <<  std::endl;
    } else {
        os << " has children :" << std::endl;
        for (int i = 0; i < _MULTIPLICITY; ++i) {
            _children[i].printSelf(os);
        }
    }
}

void NDTree::removeData(const IShape* data)
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

void NDTree::setMaxDepth(unsigned int maxDepth)
{
    if (maxDepth ==0) {
        throw std::invalid_argument("Depth of the NDTree must be at least 1");
    }
    if (maxDepth >10) {
        throw std::invalid_argument("Depth of NDTree > 10 consume too much memory");
    }
    _MAX_DEPTH = maxDepth;
}

void NDTree::setMaxStorage(unsigned int maxStorage)
{
    if (maxStorage == 0) {
        throw std::invalid_argument("MaxStorage of NDTree must be at least 1");
    }
    _MAX_STORAGE = maxStorage;
}

void NDTree::split()
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

NDTree::iterator NDTree::begin() const
{
    return iterator(*this);
}

NDTree::iterator NDTree::end() const
{
    return iterator();
}

std::ostream& operator<<(std::ostream& os, const NDTree& tree)
{
    tree.printSelf(os);
    return os;
}

NDTreeIterator::NDTreeIterator() : _node(nullptr)
{
}

NDTreeIterator::NDTreeIterator(const NDTree& node) : _node(&node)
{
    // find the leftmost leaf
    while(_node->hasChildren()) {
        _node = &_node->_children[0];
    }
}

NDTreeIterator& NDTreeIterator::operator=(const NDTreeIterator& other)
{
  _node = other._node;
  return *this;
}

bool NDTreeIterator::operator!=(const NDTreeIterator& other) const
{
    return (_node != other._node);
}

bool NDTreeIterator::operator==(const NDTreeIterator& other) const
{
    return (_node == other._node);
}

const NDTree& NDTreeIterator::operator*() const
{
    return *_node;
}

const NDTree* NDTreeIterator::operator->() const
{
    return _node;
}

NDTreeIterator& NDTreeIterator::operator++()
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

NDTreeIterator NDTreeIterator::operator++(int)
{
    NDTreeIterator tmp(*this);
    ++(*this);
    return (tmp);
}

unsigned int NDTree::numChambers() const
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

