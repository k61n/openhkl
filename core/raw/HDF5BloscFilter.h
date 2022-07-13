//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/HDF5BloscFilter.h
//! @brief     Implements class HDF5BloscFilter.
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************
#ifndef NSX_CORE_RAW_HDF5BLOSCFILTER_H
#define NSX_CORE_RAW_HDF5BLOSCFILTER_H

#include "base/parser/BloscFilter.h"
#include <memory> // unique_ptr

namespace ohkl {

/* usage:
   # include <H5Cpp.h>
   ...
   HDF5BloscFilter blosc_filter;
   H5::DSetCreatPropList plist;
   const hsize_t chunk[3] = {1, n_rows, n_cols};
   plist.setChunk(3, chunk);
   plist.setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, blosc_filter.cd_values);
*/
//! Initialize, configure and release the Blosc filter
class HDF5BloscFilter {
 public:
    //! Speed/compression for diffraction data; 0 to 3 (inclusive) param slots are reserved.
    unsigned int cd_values[7];
    //! Initialize and register the Blosc filter
    HDF5BloscFilter();

    ~HDF5BloscFilter();

 private:
    std::unique_ptr<char> _version;
    std::unique_ptr<char> _date;
    //! success flag for Blosc initialization
    bool _init_success = false;
    //! number of threads
    const std::size_t _nthreads = 4;

    //! Register BLOSC
    void _register();
};

} // namespace ohkl

#endif // NSX_CORE_RAW_HDF5BLOSCFILTER_H
