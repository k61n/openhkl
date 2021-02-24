###########################################################################
# Compiler settings
###########################################################################
#set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    set(CTEST_CONFIGURATION_TYPE "${JOB_BUILD_CONFIGURATION}")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    add_compile_options(-Wno-overlength-strings)
endif()

###########################################################################
# Dependences
###########################################################################

set(Boost_USE_MULTITHREADED ON)
# We need the Boost libraries fusion, phoenix, and spirit.
# They all are header only.
# Therefore the following find_package command goes without "COMPONENTS".
find_package(Boost 1.53.0 REQUIRED)
if(Boost_FOUND)
    include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
    message(STATUS "Found boost:")
    message(STATUS "  version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}")
    message(STATUS "  libraries: ${Boost_LIBRARIES}")
    message(STATUS "  headers: ${Boost_INCLUDE_DIRS}")
elseif(NOT Boost_FOUND)
    message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

#find_package(YAMLCPP REQUIRED)

include(CTest)

###########################################################################
# Finalize
###########################################################################

set(destination ${CMAKE_INSTALL_PREFIX})

#add_subdirectory(lib)
#add_subdirectory(test)

set(XSection_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIRECTORY}/lib PARENT_SCOPE)
