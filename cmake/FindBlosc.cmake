find_path(Blosc_INCLUDE_DIR blosc.h)

find_library(Blosc_LIBRARY NAMES blosc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Blosc DEFAULT_MSG Blosc_LIBRARY Blosc_INCLUDE_DIR)
