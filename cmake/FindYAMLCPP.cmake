#--------------------------------------------------------------------------------
# Copyright (c) 2012-2013, Lars Baehren <lbaehren@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#--------------------------------------------------------------------------------

# patches:
# - 21jul20 JWu return variables now YAMLCPP, not YAML, to comply with CMake standards,
#               and to avoid confusion with the C library libyaml.

# - Check for the presence of YAML
#
# The following variables are set when YAML is found:
#  YAMLCPP_FOUND      = Set to true, if all components of YAML have been found.
#  YAMLCPP_INCLUDES   = Include path for the header files of YAML
#  YAMLCPP_LIBRARIES  = Link these to use YAML
#  YAMLCPP_LFLAGS     = Linker flags (optional)

if(NOT YAMLCPP_FOUND)

  if(NOT YAMLCPP_ROOT_DIR)
    set (YAMLCPP_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
  endif(NOT YAMLCPP_ROOT_DIR)

  ##_____________________________________________________________________________
  ## Check for the header files

  find_path (YAMLCPP_INCLUDES yaml-cpp/yaml.h yaml-cpp/node.h
    HINTS ${YAMLCPP_ROOT_DIR} ${CMAKE_INSTALL_PREFIX}
    PATH_SUFFIXES include
    )

  ##_____________________________________________________________________________
  ## Check for the library

  find_library (YAMLCPP_LIBRARIES yaml-cpp libyaml-cppmd
                HINTS ${YAMLCPP_ROOT_DIR} ${CMAKE_INSTALL_PREFIX}
                PATH_SUFFIXES lib)

  ##_____________________________________________________________________________
  ## Actions taken when all components have been found
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args (YAMLCPP DEFAULT_MSG YAMLCPP_LIBRARIES YAMLCPP_INCLUDES)

  if(YAMLCPP_INCLUDES AND YAMLCPP_LIBRARIES)
    set (YAMLCPP_FOUND TRUE)
  else(YAMLCPP_INCLUDES AND YAMLCPP_LIBRARIES)
    set (YAMLCPP_FOUND FALSE)
    if(NOT YAMLCPP_FIND_QUIETLY)
      if(NOT YAMLCPP_INCLUDES)
	message (STATUS "Unable to find YAMLCPP header files!")
      endif(NOT YAMLCPP_INCLUDES)
      if(NOT YAMLCPP_LIBRARIES)
	message (STATUS "Unable to find YAMLCPP library files!")
      endif(NOT YAMLCPP_LIBRARIES)
    endif(NOT YAMLCPP_FIND_QUIETLY)
  endif(YAMLCPP_INCLUDES AND YAMLCPP_LIBRARIES)

  if(YAMLCPP_FOUND)
    if(NOT YAMLCPP_FIND_QUIETLY)
      message (STATUS "Found YAMLCPP:")
      message (STATUS "  libraries: ${YAMLCPP_LIBRARIES}")
      message (STATUS "  headers: ${YAMLCPP_INCLUDES}")
    endif(NOT YAMLCPP_FIND_QUIETLY)
  else(YAMLCPP_FOUND)
    if(YAMLCPP_FIND_REQUIRED)
      message (FATAL_ERROR "Could not find YAMLCPP!")
    endif(YAMLCPP_FIND_REQUIRED)
  endif(YAMLCPP_FOUND)

  ## Compatibility setting
  set (YAMLCPP_CPP_FOUND ${YAMLCPP_FOUND})

  ##_____________________________________________________________________________
  ## Mark advanced variables

  mark_as_advanced (
    YAMLCPP_ROOT_DIR
    YAMLCPP_INCLUDES
    YAMLCPP_LIBRARIES
    )

endif(NOT YAMLCPP_FOUND)
