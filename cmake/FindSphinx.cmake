# Find sphinx
#
# Usage:
#   find_package(Sphinx [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - Sphinx_FOUND          .. true if library is found
#   - SPHINX_EXECUTABLE     .. full path to library
#


find_program(SPHINX_EXECUTABLE
             NAMES sphinx-build sphinx-build2
             DOC "Path to sphinx-build executable")

# Handle REQUIRED and QUIET arguments
# this will also set SPHINX_FOUND to true if SPHINX_EXECUTABLE exists
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx
                                  "Failed to locate sphinx-build executable"
                                  SPHINX_EXECUTABLE)
mark_as_advanced(SPHINX_EXECUTABLE)

if(NOT SPHINX_FOUND)
    return()
endif()
