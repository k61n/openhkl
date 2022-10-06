//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/parser/BloscFilter.h
//! @brief     Defines blosc related things
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_PARSER_BLOSCFILTER_H
#define OHKL_BASE_PARSER_BLOSCFILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <blosc.h>

/* Filter revision number, starting at 1 */
/* #define FILTER_BLOSC_VERSION 1 */
#define FILTER_BLOSC_VERSION 2 /* multiple compressors since Blosc 1.3 */

/* Filter ID registered with the HDF Group */
#define FILTER_BLOSC 32001

/* Register the filter with the library */
int register_blosc(char** version, char** date);

#ifdef __cplusplus
}
#endif

#endif // OHKL_BASE_PARSER_BLOSCFILTER_H
