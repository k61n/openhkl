//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/DataOrder.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_DETECTOR_DATAORDER_H
#define CORE_DETECTOR_DATAORDER_H

namespace nsx {

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

#endif // CORE_DETECTOR_DATAORDER_H
