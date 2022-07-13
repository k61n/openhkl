//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/SymOp.h
//! @brief     Defines class SymOp
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_TABLES_CRYSTAL_SYMOP_H
#define NSX_TABLES_CRYSTAL_SYMOP_H

#include <Eigen/Dense>
#include <vector>

namespace ohkl {

using affineTransformation = Eigen::Transform<double, 3, Eigen::Affine>;

//! A symmetry operation of a spacegroup.

class SymOp {
 public:
    //! Default constructor should not be used but is needed for swig.
    SymOp() = default;
    //! Construct a symmetry operation by parsing a generator.
    SymOp(std::string generator);
    //! Construct symmetry operation from an Eigen affine transformatiion
    SymOp(const affineTransformation& symmetryOperation);

    bool operator==(const SymOp& other) const;
    //! Compose symmetry operations
    SymOp operator*(const SymOp& other) const;
    //! Returns 4x4 homogeneous matrix representation of the operation
    const affineTransformation& getMatrix() const;
    //! Pretty print
    void print(std::ostream& os) const;
    //! Returns the axis order
    int getAxisOrder() const;
    //! Returns the Jones symbol
    std::string getJonesSymbol() const;
    //! True if has translational part non zero
    bool hasTranslation() const;
    //! Returns true if operation is pure translation
    bool isPureTranslation() const;
    //! Returns the translation part of the symmetry operator
    Eigen::Vector3d getTranslationPart() const;
    //! Returns the rotational part of the symmetry operator
    Eigen::Matrix3d getRotationPart() const;
    //! Returns whether the translation part of 'other' is an integral multiple
    //! of the translation part of 'this'. Note: returns false if either is pure
    //! rotation. The return value, if non-zero, is the number n such that n *
    //! this->getTranslationPart() == other.getTranslationPart()
    int translationIsIntegralMultiple(const SymOp& other) const;

 private:
    affineTransformation _matrix;
};

std::ostream& operator<<(std::ostream& os, const SymOp& sym);

using SymOpList = std::vector<SymOp>;

} // namespace ohkl

#endif // NSX_TABLES_CRYSTAL_SYMOP_H
