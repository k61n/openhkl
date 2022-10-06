//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/DataOrder.h
//! @brief     Defines enum DataOrder
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_DETECTOR_DATAORDER_H
#define NSX_CORE_DETECTOR_DATAORDER_H

namespace nsx {

//! Describes how 2D images are mapped into 1D.

enum class DataOrder : size_t {
    TopLeftColMajor = 0,
    TopLeftRowMajor = 1,
    TopRightColMajor = 2,
    TopRightRowMajor = 3,
    BottomLeftColMajor = 4,
    BottomLeftRowMajor = 5,
    BottomRightColMajor = 6,
    BottomRightRowMajor = 7
};

} // namespace nsx

#endif // NSX_CORE_DETECTOR_DATAORDER_H
