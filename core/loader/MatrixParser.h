//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/MatrixParser.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_MATRIXPARSER_H
#define CORE_LOADER_MATRIXPARSER_H

#include <Eigen/Dense>

#include "core/utils/Enums.h"

// The convention for 2D data in NSXTOOL is to look at the detector
// from the sample point. In the rest position, the detector is along the beam
// (y-axis). The x-axis is horizontal and z-axis vertical, so that pixel (0,0)
// represents the bottom left and pixel (rows,cols) represents the top right.
// The functors in this file are used to do the mapping between written order
// of the data and parsing into a Eigen Matrix. For example
// TopRightColMajorMapper means that the datastream is written linearly from
// TopRight detector pixel and Column Major order (implicitly going down)

/*
 * TopRightColMajor
 * | .  .  .  .   n+1       1|
   | .  .  .  .   .         2|
   | .  .  .  .   .       ...|
   | .  .  .  .   .         n|

 * TopRightRowMajor
 * | n  .  .  .   2     1|
   | .  .  .  .   .   n+1|
   | .  .  .  .   .      |
   | .  .  .  .   .      |

 * BottomRightColMajor
 * | .  .  .  .    .     n |
   | .  .  .  .    .    ...|
   | .  .  .  .    .     2 |
   | .  .  .  .   n+1    1 |

 * BottomRightRowMajor
 * | .  .  .  .    .     . |
   | .  .  .  .    .     . |
   | .  .  .  .    .    n+1|
   | n  .  .  .    2     1 |

 * TopLeftColMajor
 * | 1  n+1 .  .    .    .|
   | 2   .  .  .    .    .|
   | .   .  .  .    .    .|
   | n   .  .  .    .    .|

 * TopLeftRowMajor
 * | 1   2  .  .    .    n|
   |n+1  .  .  .    .    .|
   | .   .  .  .    .    .|
   | .   .  .  .    .    .|

 * BottomLeftColMajor
 * | n   .  .  .    .    .|
   | .   .  .  .    .    .|
   | 2   .  .  .    .    .|
   | 1  n+1 .  .    .    .|

 * BottomLeftRowMajor
 * | .   .  .  .    .    .|
   | .   .  .  .    .    .|
   |n+1  .  .  .    .    .|
   | 1   2  .  .    .    n|
 */

namespace nsx {

//! \brief Utility class to parse real or complex matrices from plain ASCII
//! text.
class MatrixParser {

public:
    MatrixParser() = default;

    ~MatrixParser() = default;

    bool operator()(
        DataOrder dataOrder, const char* begin, size_t buffer_size, Eigen::MatrixXi& matrix) const;

    bool operator()(DataOrder dataOrder, const std::string& buffer, Eigen::MatrixXi& matrix) const;
};

} // namespace nsx

#endif // CORE_LOADER_MATRIXPARSER_H
