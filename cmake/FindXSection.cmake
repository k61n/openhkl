# Find libxsection
#
# Usage:
#   find_package(XSection [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - XSection_FOUND        .. true if library is found
#   - XSection_LIBRARIES    .. full path to library
#   - XSection_INCLUDE_DIR  .. full path to include directory
#
# Copyright 2015-2018 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

find_path(XSection_INCLUDE_DIR xsection/ChemicalFormulaParser.h)
find_library(XSection_LIBRARIES NAMES xsection XSection)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XSection DEFAULT_MSG XSection_LIBRARIES XSection_INCLUDE_DIR)
mark_as_advanced(XSection_INCLUDE_DIR XSection_LIBRARIES)

if(NOT XSection_FOUND)
    return()
endif()
include(AssertLibraryFunction)
# assert_library_function(XSection lmmin "")
