find_path(BLOSC_INCLUDE_DIR blosc.h)

find_library(BLOSC_LIBRARY NAMES blosc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BLOSC DEFAULT_MSG BLOSC_LIBRARY BLOSC_INCLUDE_DIR)
