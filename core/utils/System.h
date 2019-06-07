//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/System.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_UTILS_SYSTEM_H
#define CORE_UTILS_SYSTEM_H

// Define necessary symbols for exporting symbols in dll table.
#ifdef _WIN32
#ifdef core_EXPORT
#define DLLExport __declspec(dllexport)
#else
#define DLLEXport __declspec(dllimport)
#endif
#else
#define DLLExport
#endif

#endif // CORE_UTILS_SYSTEM_H
