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

#include "Ellipsoid.h"
#include "Octree.h"

namespace nsx {

std::vector<unsigned int> Octree::createPowers()
{
    std::vector<unsigned int> p(3);
    int i=0;
    // Powers of 2
    std::generate(p.begin(),p.end(),[&i](){return std::pow(2,i++);});

    return p;
}

Octree::Octree(Octree&& other):
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

void Octree::nullifyChildren()
{
    _children.clear();
}

Octree::Octree():
    AABB(), _depth(0), _parent(nullptr)
{
    nullifyChildren();
}

Octree::Octree(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub)
: AABB(lb,ub), 
  _depth(0), 
  _parent(nullptr)
{
    nullifyChildren();
}

Octree::Octree(const Octree* parent, unsigned int sector):
    AABB(),
    _MAX_DEPTH(parent->_MAX_DEPTH),
    _MAX_STORAGE(parent->_MAX_STORAGE),
    _depth(parent->_depth+1),
    _parent(parent),
    _idx(sector)
{
    nullifyChildren();

    // Calculate the center of the current branch
    Eigen::Vector3d center = parent->center();

    // The numbering of sub-voxels is encoded into bits of an int a follows:
    // ....... | dim[2] | dim[1] | dim[0]
    for (unsigned int i=0; i<3; ++i) {
        bool b = (sector & _POWERS[i]);
        _lowerBound(i) = (b ? center[i] : parent->_lowerBound(i));
        _upperBound(i) = (b ? parent->_upperBound(i) : center(i));
    }
}

bool Octree::addData(const Ellipsoid* ellipsoid)
{
    // Ellipsoid does not overlap with this branch
    if (!this->collide((*ellipsoid).aabb())) {
        return false;
    }

    // AABB overlap with this node
    if (hasChildren()) {
        for (auto&& child: _children) {
            child.addData(ellipsoid);
        }
    } else {
        _data.push_back(ellipsoid);
        if (_data.size() > _MAX_STORAGE) {
            split();
        }
    }
    return true;
}

bool Octree::hasChildren() const
{
    return (!_children.empty());
}

bool Octree::hasData() const
{
    return (_data.size() != 0);
}

std::set<Octree::collision_pair> Octree::getCollisions() const
{
    std::set<collision_pair> collisions;

    // loop over chambers of the ndtree
    for (auto&& chamber: *this) {
        // loop over ellipsoids in the chamber
        for (size_t i = 0; i < chamber._data.size(); ++i) {
            for (size_t j = i+1; j < chamber._data.size(); ++j) {
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

std::set<const Ellipsoid*> Octree::getCollisions(const Ellipsoid& given) const
{
    using CollisionSet = std::set<const Ellipsoid*>;

    CollisionSet collisions;

    std::function<void(const Octree*, CollisionSet&)> recursiveCollisions;

    recursiveCollisions = [&given, &recursiveCollisions] (const Octree* tree, CollisionSet& collisions) -> void
    {
        // ellipsoid's box does not intercept tree
        if (!tree->collide(given.aabb())) {
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
        for (auto&& ellipsoid: tree->_data) {
            if (ellipsoid->collide(given)) {
                collisions.emplace(ellipsoid);
            }
        }
    };

    recursiveCollisions(this, collisions);
    return collisions;
}

bool Octree::isInsideObject(const Eigen::Vector3d& vector)
{
    // chamber's box does not intercept tree
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
    for (auto&& ellipsoid: _data) {
        if (ellipsoid->isInside(vector)) {
            return true;
        }
    }

    // no collision found
    return false;
}

void Octree::getVoxels(std::vector<AABB*>& voxels)
{
    voxels.push_back(this);
    if (hasChildren()) {
        for (unsigned int i=0;i<_MULTIPLICITY;++i) {
            _children[i].getVoxels(voxels);
        }
    }
}

void Octree::printSelf(std::ostream& os) const
{
    os << "*** Node ***  " << this->_lowerBound  << "," << this->_upperBound << std::endl;
    if (!hasChildren()) {
        os << " has no children" <<std::endl;
        os << "... and has " << _data.size() << " data" <<  std::endl;
    } else {
        os << " has children :" << std::endl;
        for (size_t i = 0; i < _MULTIPLICITY; ++i) {
            _children[i].printSelf(os);
        }
    }
}

void Octree::removeData(const Ellipsoid* data)
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

void Octree::setMaxDepth(unsigned int maxDepth)
{
    if (maxDepth ==0) {
        throw std::invalid_argument("Depth of the Octree must be at least 1");
    }
    if (maxDepth >10) {
        throw std::invalid_argument("Depth of Octree > 10 consume too much memory");
    }
    _MAX_DEPTH = maxDepth;
}

void Octree::setMaxStorage(unsigned int maxStorage)
{
    if (maxStorage == 0) {
        throw std::invalid_argument("MaxStorage of Octree must be at least 1");
    }
    _MAX_STORAGE = maxStorage;
}

void Octree::split()
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

Octree::iterator Octree::begin() const
{
    return iterator(*this);
}

Octree::iterator Octree::end() const
{
    return iterator();
}

std::ostream& operator<<(std::ostream& os, const Octree& tree)
{
    tree.printSelf(os);
    return os;
}

OctreeIterator::OctreeIterator() : _node(nullptr)
{
}

OctreeIterator::OctreeIterator(const Octree& node) : _node(&node)
{
    // find the leftmost leaf
    while(_node->hasChildren()) {
        _node = &_node->_children[0];
    }
}

OctreeIterator& OctreeIterator::operator=(const OctreeIterator& other)
{
  _node = other._node;
  return *this;
}

bool OctreeIterator::operator!=(const OctreeIterator& other) const
{
    return (_node != other._node);
}

bool OctreeIterator::operator==(const OctreeIterator& other) const
{
    return (_node == other._node);
}

const Octree& OctreeIterator::operator*() const
{
    return *_node;
}

const Octree* OctreeIterator::operator->() const
{
    return _node;
}

OctreeIterator& OctreeIterator::operator++()
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

OctreeIterator OctreeIterator::operator++(int)
{
    OctreeIterator tmp(*this);
    ++(*this);
    return (tmp);
}

unsigned int Octree::numChambers() const
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

