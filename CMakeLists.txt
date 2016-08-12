cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/externals/c-blosc/cmake)


project(NSXTOOL_PROJECT)

set(NSXTOOL_VERSION_MAJOR 1)
set(NSXTOOL_VERSION_MINOR 0)

###########################################################################
# Options
###########################################################################

option(BUILD_QTAPPS "Build Qt based application." ON)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)

###########################################################################
# Scripts controlling build setup and dependencies
###########################################################################
include(CMakeFindBinUtils)
include(BuildType)             # build types: debug and release
include(CheckDependencies)     # checkexternal dependencies
include(CheckCompiler)         # configure compiler



#Set package name here
set(CPACK_PACKAGE_NAME "nsxtool")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neutron Single Crystal Diffraction Data Reduction")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "${NSXTOOL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${NSXTOOL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;" )




###########################################################################
# Now add in all the components
###########################################################################

# Build the config.h on the fly
if (UNIX)
    set(RESOURCES_DIR ${CMAKE_INSTALL_PREFIX}/share/nsxtool)
elseif (WIN32)
    set(RESOURCES_DIR "$ENV{APPDATA}\\nsxtool")
    # take care of pesky backslashes in windows path names
    STRING(REGEX REPLACE "\\\\" "\\\\\\\\" RESOURCES_DIR ${RESOURCES_DIR})
endif()
configure_file(${CMAKE_SOURCE_DIR}/cmake/Config.h.in ${CMAKE_BINARY_DIR}/include/Config.h)

add_subdirectory(nsxlib)


if (BUILD_QTAPPS)
	add_subdirectory(apps)
endif()

# additional tools
add_subdirectory(tools)

# Enable unit testing with ctest. This must be called before any calls to add_subdirectory 
enable_testing()
# Directory with all the unit-tests. 
add_subdirectory(tests)



###########################################################################
# Installation settings
###########################################################################

# Install section, installing libraries 
#install(TARGETS NSXTool DESTINATION lib)
#set(INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/NSXTool/")
#install(CODE "MESSAGE(\"Installing headers...\")")
#install(FILES ${headers} DESTINATION ${INCLUDE_DIR})




# Install the databases file
install(DIRECTORY resources/databases 
        DESTINATION ${RESOURCES_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

install(DIRECTORY resources/instruments 
        DESTINATION ${RESOURCES_DIR} 
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

###########################################################################
# CPack settings
###########################################################################
        
include(CPackCommon)
    
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

    include(CPackLinuxSetup)
    message(STATUS "CPACK_PACKAGE_FILE_NAME = ${CPACK_PACKAGE_FILE_NAME}")
      
	if ("${UNIX_DIST}" MATCHES "Ubuntu")
	    string(REPLACE "," ", " CPACK_DEBIAN_PACKAGE_DEPENDS
	    "libqt5core5a,"
	    "libqt5printsupport5,"
	    "libqt5widgets5,"
	    "libqt5gui5,"
	    "libboost-filesystem1.54.0,"
	    "libboost-date-time1.54.0,"
	    "libboost-system1.54.0,"
	    "libboost-test1.54.0,"
	    "libeigen3-dev,"
	    "libfftw3-bin,"
	    "libtiff5,"
	    "libhdf5-7 (>=1.8.11)")
	endif()

endif()

set(CMAKE_INSTALL_RPATH "/usr/local/lib")

include(CPack)
