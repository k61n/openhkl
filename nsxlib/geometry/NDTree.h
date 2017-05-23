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

class NDTreeIterator;

class NDTree : public AABB {
public:

    //! The NDTree iterator class is made friend with NDTree in order to access some of its private data
    friend class NDTreeIterator;

    // These typedefs insure compatibility with STL
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using iterator = NDTreeIterator;
    using value_type=double;
    using pointer = double*;
    using reference = double&;

    //! Pair of AABB*
    using collision_pair = std::pair<const IShape*,const IShape*>;

    //! Move constructor
    NDTree(NDTree&& other);

    //! Copy constructor
    NDTree(const NDTree& other) = delete;

    //! Assignment operator
    NDTree& operator=(const NDTree& other) = delete;

    //! Constructor from two Eigen3 vectors, throw invalid_argument if lb < ub
    NDTree(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub);

    //! Constructor from two initializer lists, throw invalid_argument if lb< ub
    NDTree(const std::initializer_list<double>& lb, const std::initializer_list<double>& ub);

    //! destructor
    ~NDTree()=default;

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
    void getVoxels(std::vector<AABB*>& voxels);

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
    const unsigned int _MULTIPLICITY = std::pow(2,3);

    //! vector of powers 2^D
    std::vector<unsigned int> _POWERS = createPowers();

    //! Vector of 2^D children
    std::vector<NDTree> _children;
    // NDTree<T,D>* _children[getPow(D)];

    //! Vector of data object in this leaf
    std::vector<const IShape*> _data;

    //! Depth of this branch with respect to root node.
    unsigned int _depth;

    const NDTree* _parent;
    long _idx = -1;
};

class NDTreeIterator
{
public:
    //; The default constructor. Used only for end condition.
    NDTreeIterator();

    //; The constructor from a reference to the NDTree to be iterated.
    NDTreeIterator(const NDTree& tree);

    NDTreeIterator& operator=(const NDTreeIterator& other);

    bool operator==(const NDTreeIterator& other) const;

    bool operator!=(const NDTreeIterator& other) const;

    const NDTree& operator*() const;

    const NDTree* operator->() const;

    NDTreeIterator& operator++();

    NDTreeIterator operator++(int);

private:

    const NDTree* _node;
};

} // end namespace nsx

#endif // NSXLIB_NDTREE_H
