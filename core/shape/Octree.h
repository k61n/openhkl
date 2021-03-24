//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/Octree.h
//! @brief     Defines classes Octree, OctreeIterator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_SHAPE_OCTREE_H
#define NSX_CORE_SHAPE_OCTREE_H

#include "base/geometry/AABB.h"
#include <set>

namespace nsx {

class OctreeIterator;

//! A binary tree in D dimensions, used to speed up collision detection.

//! Octree is used to partition space in D dimensions and employed for fast
//! sorting or collision detections of AABB objects. NdTree derived from AABB
//! since each voxel is a bounded object itself. The root node has dimensions of
//! the full world. The Octree instance does not own the AABB* objects: since
//! users deals with the lifetime of AABB* objects outside of the class, one must
//! ensure that these have a greater lifetime than that of NdTree. Octree define
//! a soft constraint on the the maximum number of AABB* objects that can be
//! stored in each leaf of the tree (_MAX_STORAGE), and a maximal depth for each
//! branch (_MAX_DEPTH). If the number of subdivisions reaches _MAX_DEPTH, the
//! soft constraint on _MAX_STORAGE is broken.

class Octree : public AABB {
 public:
    friend class OctreeIterator;

    // These typedefs insure compatibility with STL
    using iterator = OctreeIterator;
    //! Pair of Ellipsoid*
    using collision_pair = std::pair<const Ellipsoid*, const Ellipsoid*>;

    //! Constructor from two Eigen3 vectors, throw invalid_argument if lb < ub
    Octree(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub);
    Octree(const Octree* parent, unsigned int sector);
    ~Octree() = default;


    Octree(Octree&& other) noexcept;
    Octree(const Octree& other) = delete;


    //! Add a new AABB object to the deepest leaf.
    //! If the leaf has reached capacity of _MAX_STORAGE, then it will be split
    //! into 2^D sub-Octree, unless _MAX_DEPTH is reached, in which case data will
    //! simply be added to this leaf. Returns true if the ellipsoid was added and
    //! false otherwise.
    bool addData(const Ellipsoid* ellipsoid);

    //! Returns true if the node has some children
    bool hasChildren() const;

    //! Returns true if the node has some data
    bool hasData() const;

    //! Recursively send some information about the node (and children) to a
    //! stream
    void printSelf(std::ostream& os) const;

    //! Change the _MAX_DEPTH property of the Tree
    void setMaxDepth(unsigned int maxDepth);

    //! Change the _MAX_STORAGE property of the Tree
    void setMaxStorage(unsigned int maxStorage);

    //! Returns the list of AABB* pairs that intercept.
    // void getPossibleCollisions(std::set<collision_pair>& collisions) const;
    std::set<collision_pair> getCollisions() const;

    //! Returns collisions with a given shape
    std::set<const Ellipsoid*> getCollisions(const Ellipsoid& given) const;

    //! Returns true if the point is contained in any object of the octree
    bool isInsideObject(const Eigen::Vector3d& vector) const;

    //! Gets the voxels of the tree
    void getVoxels(std::vector<AABB*>& voxels);

    //! Remove a data from the Octree
    void removeData(const Ellipsoid* data);

    iterator begin() const;

    iterator end() const;

    const std::vector<const Ellipsoid*>& getData() const { return _data; }

    unsigned int numChambers() const;

 private:
    //! Prevent defining tree with null world.
    Octree();

    //! Sets all children to nullptr
    void nullifyChildren();

    //! Split the node into 2^D subnodes
    void split();

    //! Method to initialize vector of powers 2^D
    static std::vector<unsigned int> createPowers();

    //! Maximum number of recursive splits of Octree
    unsigned int _MAX_DEPTH = 5;

    //! Maximum number of AABB* data object stored in each leaf
    unsigned int _MAX_STORAGE = 5;

    //! Multiplicity of the tree branch (D=2 ->4), (D=3 ->8)
    const unsigned int _MULTIPLICITY = unsigned(std::pow(2, 3));

    //! vector of powers 2^D
    std::vector<unsigned int> _POWERS = createPowers();

    //! Vector of 2^D children
    std::vector<Octree> _children;

    //! Vector of data object in this leaf
    std::vector<const Ellipsoid*> _data;

    //! Depth of this branch with respect to root node.
    unsigned int _depth;

    const Octree* _parent;

    unsigned int _idx = 0;
};

//! Iterator for Octree leaves/chambers.

class OctreeIterator {
 public:
    //! The default constructor. Used only for end condition.
    OctreeIterator();

    //! Copy constructor, used for iteration(?)
    OctreeIterator(const Octree& node);

    bool operator==(const OctreeIterator& other) const;
    bool operator!=(const OctreeIterator& other) const;

    const Octree& operator*() const;
    const Octree* operator->() const;
    OctreeIterator& operator++();

 private:
    const Octree* _node;
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_OCTREE_H
