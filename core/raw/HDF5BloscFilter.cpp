//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/HDF5BloscFilter.cpp
//! @brief     Defines class HDF5BloscFilter.
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/raw/HDF5BloscFilter.h"
#include "base/utils/Logger.h"

#include <stdexcept>

namespace ohkl {

HDF5BloscFilter::HDF5BloscFilter()
{
    blosc_init();
    _init_success = true;
    blosc_set_nthreads(_nthreads);
    _register();

    // speed/compression for diffraction data
    cd_values[4] = 9; // Highest compression level
    cd_values[5] = 1; // Bit shuffling active; 0: shuffle not active, 1: shuffle active
    cd_values[6] = BLOSC_BLOSCLZ; // Actual compressor to use: BLOSC seem to be the best compromise
    nsxlog(ohkl::Level::Debug, "Initialized Blosc-filter to read HDF5 data");
}

HDF5BloscFilter::~HDF5BloscFilter()
{
    if (_init_success) {
        blosc_destroy();
        nsxlog(ohkl::Level::Debug, "Released Blosc-filter resources");
    }
}

void HDF5BloscFilter::_register()
{
    char *ptrVersion = _version.get(), *ptrDate = _date.get();
    const int register_status = register_blosc(&ptrVersion, &ptrDate);
    if (register_status <= 0)
        throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

    /* NOTE:
       BLOSC register_status stores the version and the date with `strdup`
       *version = strdup(BLOSC_VERSION_STRING);
       *date = strdup(BLOSC_VERSION_DATE);

       Therefore version and date must be freed afterwards.
    */
}

} // namespace ohkl
