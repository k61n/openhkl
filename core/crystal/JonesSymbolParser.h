#pragma once

#include <Eigen/Dense>

namespace nsx {

//! Function to parse Jones faithful representation of affine transformations
//! written as strings of the form 2x+1/3,y-z+1,x+1/4 Return an affine transform
//! matrix which represents the rotational and translational parts of the
//! symmetry operator. This parser only validates the grammar but not the
//! Symmetry operation itself. Valid terms are signed linear terms in x, y, or z
//! and constants,either integer or fractional numbers written as n/d. For
//! example, the symbol x,y,z will be parsed as the identity matrix and no
//! translational part. The symbol -y,-x,z+1/2 will be parsed into the matrix
//!  0,-1, 0, 0
//!  1, 0, 0, 0
//!  0, 0, 1, 0.5,
//!  0, 0, 0, 1
Eigen::Transform<double, 3, Eigen::Affine>
parseJonesSymbol(const std::string &jonesSymbol);

} // end namespace nsx
