//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/LogLevel.h
//! @brief     Defines struct Level as the log level
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_UTILS_LOGLEVEL_H
#define OHKL_BASE_UTILS_LOGLEVEL_H

namespace ohkl {

//! Verbosity of the logger
enum class Level {
    Off = 0,
    Info,
    Error,
    Warning,
    Debug,
};

} // namespace ohkl

#endif // OHKL_BASE_UTILS_LOGLEVEL_H
