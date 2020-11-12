find_path(QHULL_INCLUDE_DIR NAMES libqhullcpp/Qhull.h Qhull.h)

find_library(QHULL_LIBRARY1 NAMES qhull_r)
find_library(QHULL_LIBRARY2 NAMES qhullcpp)

list(APPEND QHULL_LIBRARIES "${QHULL_LIBRARY1}")
list(APPEND QHULL_LIBRARIES "${QHULL_LIBRARY2}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Qhull DEFAULT_MSG QHULL_LIBRARIES QHULL_INCLUDE_DIR)
mark_as_advanced(QHULL_INCLUDE_DIR QHULL_LIBRARIES)
